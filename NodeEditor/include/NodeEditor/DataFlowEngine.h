#pragma once

#include <QObject>
#include <QList>
#include <QUuid>
#include <QMap>
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

    void processNodeChange(const QUuid &nodeId);
    Q_INVOKABLE void processAll();

    void clearCache();

signals:
    void propagationComplete(QList<QUuid> computedNodes);
    void cycleDetected();
    void graphModelChanged();
    void autoComputeChanged();

private:
    BaseNode *getOrCreateNode(const QString &type);
    void executeNode(const QUuid &nodeId);
    void wireConnections();
    void unwireConnections();

    GraphModel *m_model = nullptr;
    QMap<QString, BaseNode*> m_nodeInstances;
    bool m_processing = false;
    bool m_autoCompute = true;
    QList<QMetaObject::Connection> m_connections;
};

} // namespace NodeEditor
