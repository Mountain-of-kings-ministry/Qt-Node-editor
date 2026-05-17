#pragma once

#include <QObject>
#include <QHash>
#include <QString>
#include <QList>
#include <QSet>
#include <QMutex>
#include <QPointer>
#include <QImage>
#include <QTimer>
#include "NodeEditor/GraphModel.h"

namespace NodeEditor {

class DataFlowEngine;
class BaseNode;

struct PreviewEntry {
    QString base64;
    uint64_t version = 0;
    bool dirty = false;
};

class PreviewManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int renderBudget READ renderBudget WRITE setRenderBudget NOTIFY renderBudgetChanged)
    Q_PROPERTY(int maxCacheSize READ maxCacheSize WRITE setMaxCacheSize NOTIFY maxCacheSizeChanged)
    Q_PROPERTY(double previewScale READ previewScale WRITE setPreviewScale NOTIFY previewScaleChanged)
    Q_PROPERTY(bool interacting READ isInteracting WRITE setInteracting NOTIFY interactingChanged)
public:
    explicit PreviewManager(QObject *parent = nullptr);
    explicit PreviewManager(GraphModel *model, DataFlowEngine *engine, QObject *parent = nullptr);
    ~PreviewManager() override;

    void setModel(GraphModel *model);
    GraphModel *model() const;
    void setEngine(DataFlowEngine *engine);
    DataFlowEngine *engine() const;

    int renderBudget() const;
    void setRenderBudget(int budget);

    int maxCacheSize() const;
    void setMaxCacheSize(int size);

    Q_INVOKABLE void requestPreview(const QString &nodeId);
    Q_INVOKABLE QString previewData(const QString &nodeId) const;
    Q_INVOKABLE bool hasPreview(const QString &nodeId) const;

    void cachePreview(uint64_t nodeId, const QString &base64, uint64_t version);
    Q_INVOKABLE void processRequests();
    Q_INVOKABLE void asyncRenderNode(const QString &nodeId);

    // Direct render: calls BaseNode::render() on current thread, caches result
    void renderNode(uint64_t nodeId);

    double previewScale() const;
    void setPreviewScale(double scale);

    bool isInteracting() const;
    Q_INVOKABLE void setInteracting(bool interacting);
    Q_INVOKABLE void setTemporarilyInteracting();

signals:
    void previewReady(uint64_t nodeId, QString base64);
    void qmlPreviewReady(QString nodeId);
    void renderBudgetChanged();
    void maxCacheSizeChanged();
    void previewScaleChanged();
    void interactingChanged();

private:
    void evictLRU();
    void onNodeDataChanged(uint64_t nodeId, const QString &key);

    QPointer<GraphModel> m_model;
    DataFlowEngine *m_engine = nullptr;
    QHash<uint64_t, PreviewEntry> m_cache;
    QList<uint64_t> m_accessOrder;
    QSet<uint64_t> m_pending;
    int m_renderBudget = 3;
    int m_maxCacheSize = 50;
    mutable QMutex m_mutex;
    QMetaObject::Connection m_dataConnection;
    QHash<QString, BaseNode*> m_instances;
    QMutex m_instanceMutex;
    double m_previewScale = 1.0;
    bool m_interacting = false;
    QTimer *m_idleTimer = nullptr;
};

} // namespace NodeEditor
