#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QPointF>

class QUndoStack;

namespace NodeEditor {

class GraphModel;

class UndoManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
public:
    explicit UndoManager(GraphModel *model, QObject *parent = nullptr);
    ~UndoManager() override;

    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void clear();

    Q_INVOKABLE QString qmlAddNode(const QString &type, double x, double y);
    Q_INVOKABLE void qmlRemoveNode(const QString &nodeId);
    Q_INVOKABLE QString qmlConnectPorts(const QString &sourceNode, const QString &sourcePort,
                                         const QString &targetNode, const QString &targetPort);
    Q_INVOKABLE void qmlDisconnectEdge(const QString &edgeId);
    Q_INVOKABLE void qmlMoveNode(const QString &nodeId, double oldX, double oldY, double newX, double newY);
    Q_INVOKABLE void qmlSetNodeData(const QString &nodeId, const QString &key,
                                     const QVariant &oldValue, const QVariant &newValue);

    bool canUndo() const;
    bool canRedo() const;

signals:
    void canUndoChanged();
    void canRedoChanged();

private:
    GraphModel *m_model;
    QUndoStack *m_stack;
};

} // namespace NodeEditor
