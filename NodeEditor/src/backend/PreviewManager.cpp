#include "NodeEditor/PreviewManager.h"
#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/BaseNode.h"
#include <QBuffer>
#include <QtConcurrent/QtConcurrentRun>

namespace NodeEditor {

static QString imageToBase64(const QImage &img)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    return QString::fromLatin1(bytes.toBase64());
}

PreviewManager::PreviewManager(QObject *parent)
    : QObject(parent)
{
    m_idleTimer = new QTimer(this);
    m_idleTimer->setSingleShot(true);
    m_idleTimer->setInterval(500);
    connect(m_idleTimer, &QTimer::timeout, this, [this]() {
        setInteracting(false);
    });
}

PreviewManager::PreviewManager(GraphModel *model, DataFlowEngine *engine,
                               QObject *parent)
    : QObject(parent), m_engine(engine)
{
    setModel(model);
}

PreviewManager::~PreviewManager()
{
    disconnect(m_dataConnection);
}

void PreviewManager::setModel(GraphModel *model)
{
    disconnect(m_dataConnection);
    m_model = model;
    if (model) {
        m_dataConnection = connect(model, &GraphModel::nodeDataChanged, this,
            &PreviewManager::onNodeDataChanged);
    }
}

GraphModel *PreviewManager::model() const { return m_model.data(); }

void PreviewManager::setEngine(DataFlowEngine *engine) { m_engine = engine; }
DataFlowEngine *PreviewManager::engine() const { return m_engine; }

int PreviewManager::renderBudget() const { return m_renderBudget; }

void PreviewManager::setRenderBudget(int budget)
{
    if (budget < 1) budget = 1;
    if (budget == m_renderBudget) return;
    m_renderBudget = budget;
    emit renderBudgetChanged();
}

int PreviewManager::maxCacheSize() const { return m_maxCacheSize; }

void PreviewManager::setMaxCacheSize(int size)
{
    if (size < 1) size = 1;
    if (size == m_maxCacheSize) return;
    m_maxCacheSize = size;
    evictLRU();
    emit maxCacheSizeChanged();
}

void PreviewManager::onNodeDataChanged(uint64_t nodeId, const QString &key)
{
    if (key != QStringLiteral("display")) return;
    if (!m_model) return;

    auto *rt = m_model->nodeRuntime(nodeId);
    uint64_t version = rt ? rt->evalVersion : 0;

    QMutexLocker lock(&m_mutex);
    auto it = m_cache.find(nodeId);
    if (it != m_cache.end() && it.value().version >= version)
        return;
    lock.unlock();

    QVariant val = m_model->nodeData(nodeId, key);
    if (val.isValid())
        cachePreview(nodeId, val.toString(), version);
}

void PreviewManager::requestPreview(const QString &nodeId)
{
    uint64_t id = strToId(nodeId);
    if (!id) return;

    QMutexLocker lock(&m_mutex);
    if (m_cache.contains(id) && !m_cache[id].dirty) {
        lock.unlock();
        emit previewReady(id, m_cache[id].base64);
        emit qmlPreviewReady(nodeId);
        return;
    }
    m_pending.insert(id);
}

QString PreviewManager::previewData(const QString &nodeId) const
{
    uint64_t id = strToId(nodeId);
    QMutexLocker lock(&m_mutex);
    auto it = m_cache.find(id);
    if (it == m_cache.end()) return {};
    return it.value().base64;
}

bool PreviewManager::hasPreview(const QString &nodeId) const
{
    uint64_t id = strToId(nodeId);
    QMutexLocker lock(&m_mutex);
    return m_cache.contains(id);
}

void PreviewManager::cachePreview(uint64_t nodeId, const QString &base64,
                                   uint64_t version)
{
    QMutexLocker lock(&m_mutex);

    auto it = m_cache.find(nodeId);
    if (it != m_cache.end()) {
        if (it.value().version >= version) return;
        it.value().base64 = base64;
        it.value().version = version;
        it.value().dirty = false;
    } else {
        PreviewEntry entry;
        entry.base64 = base64;
        entry.version = version;
        m_cache.insert(nodeId, entry);
    }

    m_accessOrder.removeAll(nodeId);
    m_accessOrder.prepend(nodeId);
    evictLRU();

    lock.unlock();

    emit previewReady(nodeId, base64);
    emit qmlPreviewReady(idToStr(nodeId));
}

void PreviewManager::processRequests()
{
    QMutexLocker lock(&m_mutex);

    if (m_pending.isEmpty()) return;

    int processed = 0;
    QList<uint64_t> toProcess;

    auto it = m_pending.begin();
    while (it != m_pending.end() && processed < m_renderBudget) {
        toProcess.append(*it);
        it = m_pending.erase(it);
        processed++;
    }

    lock.unlock();

    for (uint64_t id : toProcess) {
        if (m_engine) {
            m_engine->requestValue(id, QStringLiteral("display"));
        } else if (m_model) {
            auto *rt = m_model->nodeRuntime(id);
            uint64_t ver = rt ? rt->evalVersion : 0;
            QVariant val = m_model->nodeData(id, QStringLiteral("display"));
            if (val.isValid())
                cachePreview(id, val.toString(), ver);
        }
    }
}

double PreviewManager::previewScale() const { return m_previewScale; }

void PreviewManager::setPreviewScale(double scale)
{
    if (scale < 0.1) scale = 0.1;
    if (scale > 2.0) scale = 2.0;
    if (qFuzzyCompare(scale, m_previewScale)) return;
    m_previewScale = scale;
    emit previewScaleChanged();
}

bool PreviewManager::isInteracting() const { return m_interacting; }

void PreviewManager::setInteracting(bool interacting)
{
    if (interacting == m_interacting) return;
    m_interacting = interacting;
    m_idleTimer->stop();
    if (interacting) {
        setPreviewScale(0.5);
    } else {
        setPreviewScale(1.0);
        processRequests();
    }
    emit interactingChanged();
}

void PreviewManager::setTemporarilyInteracting()
{
    setInteracting(true);
    m_idleTimer->start();
}

void PreviewManager::evictLRU()
{
    while (m_cache.size() > m_maxCacheSize && !m_accessOrder.isEmpty()) {
        uint64_t last = m_accessOrder.takeLast();
        m_cache.remove(last);
        m_pending.remove(last);
    }
}

void PreviewManager::renderNode(uint64_t nodeId)
{
    if (!m_model) return;

    const auto *gn = m_model->graphNode(nodeId);
    if (!gn) return;

    BaseNode *instance = nullptr;
    {
        QMutexLocker lock(&m_instanceMutex);
        auto it = m_instances.find(gn->type);
        if (it != m_instances.end()) {
            instance = it.value();
        } else {
            instance = BaseNode::create(gn->type);
            if (instance) m_instances[gn->type] = instance;
        }
    }
    if (!instance) return;

    QVariantMap inputs;
    for (auto it = gn->inputs.begin(); it != gn->inputs.end(); ++it) {
        QVariant val = m_model->nodeData(nodeId, it.key());
        if (val.isValid()) {
            inputs[it.key()] = val;
        } else {
            inputs[it.key()] = it.value().defaultValue;
        }
    }

    QSize baseSize(256, 256);
    QSize scaledSize(qMax(1, int(baseSize.width() * m_previewScale)),
                     qMax(1, int(baseSize.height() * m_previewScale)));
    QImage img = instance->render(inputs, scaledSize);
    if (img.isNull()) return;

    auto *rt = m_model->nodeRuntime(nodeId);
    uint64_t version = rt ? rt->evalVersion : 0;
    cachePreview(nodeId, imageToBase64(img), version);
}

void PreviewManager::asyncRenderNode(const QString &nodeId)
{
    uint64_t id = strToId(nodeId);
    if (!id) return;

    QPointer<PreviewManager> guard(this);
    (void)QtConcurrent::run([guard, id]() {
        if (!guard) return;
        guard->renderNode(id);
    });
}

} // namespace NodeEditor
