#include "NodeEditor/AsyncEvaluator.h"
#include "NodeEditor/BaseNode.h"
#include <QtConcurrent/QtConcurrentRun>

namespace NodeEditor {

AsyncEvaluator::AsyncEvaluator(QObject *parent)
    : QObject(parent)
{
    m_pool.setMaxThreadCount(m_maxThreads);
}

AsyncEvaluator::AsyncEvaluator(GraphModel *model, QObject *parent)
    : QObject(parent), m_model(model)
{
    m_pool.setMaxThreadCount(m_maxThreads);
}

AsyncEvaluator::~AsyncEvaluator()
{
    m_pool.waitForDone();
}

void AsyncEvaluator::setGraphModel(GraphModel *model)
{
    if (m_model == model) return;
    m_pool.waitForDone();
    m_model = model;
}

GraphModel *AsyncEvaluator::graphModel() const { return m_model; }

int AsyncEvaluator::maxThreads() const { return m_maxThreads; }

void AsyncEvaluator::setMaxThreads(int n)
{
    if (n < 1) n = 1;
    if (n == m_maxThreads) return;
    m_maxThreads = n;
    m_pool.setMaxThreadCount(n);
    emit maxThreadsChanged();
}

bool AsyncEvaluator::isBusy() const { return m_busy; }

// ── Public API ─────────────────────────────────────────────────

void AsyncEvaluator::evaluateDirty(const QSet<uint64_t> &dirtyNodes)
{
    if (!m_model || dirtyNodes.isEmpty() || m_busy) return;
    m_busy = true;
    emit busyChanged();
    emit started();

    auto snapshot = QSharedPointer<EvalSnapshot>::create();
    buildSnapshot(dirtyNodes, 0, snapshot);
    kickLevel(0, snapshot);
}

void AsyncEvaluator::evaluateFrom(uint64_t nodeId, const QSet<uint64_t> &downstream)
{
    if (!m_model || m_busy) return;
    m_busy = true;
    emit busyChanged();
    emit started();

    auto snapshot = QSharedPointer<EvalSnapshot>::create();
    buildSnapshot(downstream, nodeId, snapshot);
    kickLevel(0, snapshot);
}

void AsyncEvaluator::qmlEvaluateDirty(const QStringList &dirtyIds)
{
    QSet<uint64_t> ids;
    for (const QString &s : dirtyIds)
        ids.insert(strToId(s));
    evaluateDirty(ids);
}

void AsyncEvaluator::qmlEvaluateFrom(const QString &nodeId)
{
    uint64_t id = strToId(nodeId);
    if (!id) return;
    if (!m_model) return;

    // Compute downstream set on caller thread
    QSet<uint64_t> downstream;
    downstream.insert(id);
    QList<uint64_t> queue = {id};
    while (!queue.isEmpty()) {
        uint64_t cur = queue.takeFirst();
        for (const auto &e : m_model->allGraphEdges()) {
            if (e.sourceNodeId == cur && !downstream.contains(e.targetNodeId)) {
                downstream.insert(e.targetNodeId);
                queue.append(e.targetNodeId);
            }
        }
    }
    evaluateFrom(id, downstream);
}

// ── Build snapshot ────────────────────────────────────────────

void AsyncEvaluator::buildSnapshot(const QSet<uint64_t> &dirtyNodes,
                                   uint64_t changeRoot,
                                   QSharedPointer<EvalSnapshot> &snapshot)
{
    snapshot->nodes = m_model->allGraphNodes();
    snapshot->edges = m_model->allGraphEdges();
    snapshot->dirty = dirtyNodes;
    snapshot->changeRoot = changeRoot;

    for (auto it = snapshot->nodes.begin(); it != snapshot->nodes.end(); ++it) {
        QVariantMap portData;
        const auto *rt = m_model->nodeRuntime(it.key());
        if (rt) portData = rt->data;
        snapshot->data.insert(it.key(), portData);
    }

    snapshot->levels = m_model->topologicalLevels();
}

// ── Kick off a level ──────────────────────────────────────────

void AsyncEvaluator::kickLevel(int level, QSharedPointer<EvalSnapshot> snapshot)
{
    if (level >= snapshot->levels.size()) {
        // All levels done — merge results back
        QList<uint64_t> computed;
        {
            QMutexLocker lock(&snapshot->resultMutex);
            computed = snapshot->computedOrder;
        }
        mergeResults(snapshot, computed);
        m_busy = false;
        emit busyChanged();
        emit finished(computed);

        QStringList strIds;
        strIds.reserve(computed.size());
        for (uint64_t id : computed)
            strIds.append(idToStr(id));
        emit qmlFinished(strIds);
        return;
    }

    emit levelStarted(level, snapshot->levels.size());

    const auto &nodesAtLevel = snapshot->levels[level];
    if (nodesAtLevel.isEmpty()) {
        kickLevel(level + 1, snapshot);
        return;
    }

    QAtomicInt *remaining = new QAtomicInt(nodesAtLevel.size());

    for (uint64_t nodeId : nodesAtLevel) {
        (void)QtConcurrent::run(&m_pool, [this, nodeId, snapshot, level, remaining]() {
            doEvalNode(nodeId, snapshot);

            if (remaining->fetchAndAddOrdered(-1) == 1) {
                // Last node in this level — advance to next level on main thread
                delete remaining;
                QMetaObject::invokeMethod(this, [this, level, snapshot]() {
                    emit levelFinished(level);
                    kickLevel(level + 1, snapshot);
                }, Qt::QueuedConnection);
            }
        });
    }
}

// ── Evaluate one node (runs on worker thread) ─────────────────

void AsyncEvaluator::doEvalNode(uint64_t nodeId,
                                QSharedPointer<EvalSnapshot> snapshot)
{
    const auto gnIt = snapshot->nodes.find(nodeId);
    if (gnIt == snapshot->nodes.end()) return;

    const GraphNode &gn = gnIt.value();

    // Get or create node instance
    BaseNode *instance = nullptr;
    {
        QMutexLocker lock(&m_instanceMutex);
        auto instIt = m_instances.find(gn.type);
        if (instIt != m_instances.end()) {
            instance = instIt.value();
        } else {
            instance = BaseNode::create(gn.type);
            if (instance) m_instances[gn.type] = instance;
        }
    }

    if (!instance) {
        QMutexLocker lock(&snapshot->resultMutex);
        snapshot->computedOrder.append(nodeId);
        return;
    }

    // Gather inputs from snapshot data
    QVariantMap inputs;
    for (auto it = gn.inputs.begin(); it != gn.inputs.end(); ++it) {
        const QString &portName = it.key();
        bool connected = false;

        for (const auto &e : snapshot->edges) {
            if (e.targetNodeId == nodeId && e.targetPort == portName) {
                auto dataIt = snapshot->data.find(e.sourceNodeId);
                if (dataIt != snapshot->data.end()) {
                    QVariant srcVal = dataIt.value().value(e.sourcePort);
                    if (srcVal.isValid()) {
                        inputs[portName] = srcVal;
                        connected = true;
                    }
                }
                break;
            }
        }

        if (!connected) {
            auto dataIt = snapshot->data.find(nodeId);
            QVariant stored = dataIt != snapshot->data.end()
                              ? dataIt.value().value(portName) : QVariant();
            inputs[portName] = stored.isValid() ? stored : it.value().defaultValue;
        }
    }

    // Check dirty: skip if node is clean (not dirty and not change root)
    if (!snapshot->dirty.contains(nodeId) && nodeId != snapshot->changeRoot) {
        QMutexLocker lock(&snapshot->resultMutex);
        snapshot->computedOrder.append(nodeId);
        return;
    }

    // Compute on worker thread
    QVariantMap outputs;
    try {
        outputs = instance->compute(inputs);
    } catch (...) {
        QMutexLocker lock(&snapshot->resultMutex);
        snapshot->computedOrder.append(nodeId);
        const QString errMsg = QStringLiteral("Exception in node %1").arg(gn.type);
        QMetaObject::invokeMethod(this, [this, nodeId, errMsg]() {
            emit nodeError(nodeId, errMsg);
        }, Qt::QueuedConnection);
        return;
    }

    // Store results
    {
        QMutexLocker lock(&snapshot->resultMutex);
        auto &nodeResults = snapshot->results[nodeId];
        for (auto oit = outputs.begin(); oit != outputs.end(); ++oit)
            nodeResults[oit.key()] = oit.value();

        snapshot->computedOrder.append(nodeId);
    }
}

// ── Merge results back to GraphModel on main thread ───────────

void AsyncEvaluator::mergeResults(QSharedPointer<EvalSnapshot> snapshot,
                                   const QList<uint64_t> &computed)
{
    if (!m_model) return;

    for (uint64_t nodeId : computed) {
        auto resIt = snapshot->results.find(nodeId);
        if (resIt == snapshot->results.end()) continue;

        for (auto it = resIt.value().begin(); it != resIt.value().end(); ++it)
            m_model->setNodeData(nodeId, it.key(), it.value());
    }
}

} // namespace NodeEditor
