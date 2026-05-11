#pragma once

#include <QObject>
#include <QList>
#include <QUuid>
#include <QMap>

namespace NodeEditor {

class GraphModel;
class BaseNode;

class DataFlowEngine : public QObject {
    Q_OBJECT
public:
    explicit DataFlowEngine(GraphModel *model, QObject *parent = nullptr);

    void processNodeChange(const QUuid &nodeId);
    void processAll();

    void clearCache();

signals:
    void propagationComplete(QList<QUuid> computedNodes);
    void cycleDetected();

private:
    BaseNode *getOrCreateNode(const QString &type);
    void executeNode(const QUuid &nodeId);

    GraphModel *m_model;
    QMap<QString, BaseNode*> m_nodeInstances;
};

} // namespace NodeEditor
