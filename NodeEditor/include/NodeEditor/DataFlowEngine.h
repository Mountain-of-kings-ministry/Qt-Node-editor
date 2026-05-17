#pragma once

#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>
#include <QString>
#include <QMetaObject>

namespace NodeEditor {

class GraphModel;
class BaseNode;

class DataFlowEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(NodeEditor::GraphModel *graphModel READ graphModel WRITE setGraphModel NOTIFY graphModelChanged)
    Q_PROPERTY(bool autoCompute READ autoCompute WRITE setAutoCompute NOTIFY autoComputeChanged)
public:
    explicit DataFlowEngine(QObject *parent = nullptr);
    explicit DataFlowEngine(GraphModel *model, QObject *parent = nullptr);

    GraphModel *graphModel() const;
    void setGraphModel(GraphModel *model);

    bool autoCompute() const;
    void setAutoCompute(bool enabled);

    void processNodeChange(uint64_t nodeId);
    Q_INVOKABLE void processAll();

    // Pull evaluation: compute only the upstream nodes needed for a port value
    QVariant requestValue(uint64_t nodeId, const QString &port);
    Q_INVOKABLE QVariant qmlRequestValue(const QString &nodeId, const QString &port);

    void clearCache();

signals:
    void propagationComplete(QList<uint64_t> computedNodes);
    void cycleDetected();
    void graphModelChanged();
    void autoComputeChanged();

private:
    BaseNode *getOrCreateNode(const QString &type);
    bool executeNode(uint64_t nodeId, QList<uint64_t> &computedOrder);
    void wireConnections();
    void unwireConnections();

    GraphModel *m_model = nullptr;
    QHash<QString, BaseNode*> m_nodeInstances;
    bool m_processing = false;
    bool m_autoCompute = true;
    QList<QMetaObject::Connection> m_connections;

    // Dirty DAG tracking
    QSet<uint64_t> m_dirtyNodes;
};

} // namespace NodeEditor
