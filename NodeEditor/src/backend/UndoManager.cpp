#include "NodeEditor/UndoManager.h"
#include "NodeEditor/GraphModel.h"
#include <QUndoStack>
#include <QUndoCommand>

namespace NodeEditor {

static const int MOVE_NODE_ID = 1001;
static const int SET_DATA_ID = 1002;

// ── AddNodeCommand ───────────────────────────────────────────

class AddNodeCommand : public QUndoCommand {
    GraphModel *m_model;
    QString m_type;
    QPointF m_position;
    QString m_nodeId;
public:
    AddNodeCommand(GraphModel *model, const QString &type, const QPointF &pos)
        : m_model(model), m_type(type), m_position(pos)
    {
        setText(QStringLiteral("Add %1 node").arg(type));
    }

    void undo() override {
        if (!m_nodeId.isEmpty())
            m_model->qmlRemoveNode(m_nodeId);
    }

    void redo() override {
        QUuid id = m_model->addNode(m_type, m_position,
            m_nodeId.isEmpty() ? QUuid() : GraphModel::strToUuid(m_nodeId));
        m_nodeId = GraphModel::uuidToStr(id);
    }

    QString nodeId() const { return m_nodeId; }
};

// ── RemoveNodeCommand ────────────────────────────────────────

class RemoveNodeCommand : public QUndoCommand {
    GraphModel *m_model;
    QString m_nodeId;
    QString m_type;
    QPointF m_position;
    QVariantMap m_data;
    struct EdgeSnapshot {
        QString sourceNode, sourcePort, targetNode, targetPort;
    };
    QList<EdgeSnapshot> m_edges;
public:
    RemoveNodeCommand(GraphModel *model, const QString &nodeId)
        : m_model(model), m_nodeId(nodeId)
    {
        setText(QStringLiteral("Remove node"));
        auto *n = model->node(GraphModel::strToUuid(nodeId));
        if (n) {
            m_type = n->type;
            m_position = n->position;
            m_data = n->data;
        }
        for (const auto &e : model->edges()) {
            QString eSrc = GraphModel::uuidToStr(e.sourceNodeId);
            QString eTgt = GraphModel::uuidToStr(e.targetNodeId);
            if (eSrc == nodeId || eTgt == nodeId)
                m_edges.append({eSrc, e.sourcePort, eTgt, e.targetPort});
        }
    }

    void undo() override {
        m_model->addNode(m_type, m_position, GraphModel::strToUuid(m_nodeId));
        for (auto it = m_data.begin(); it != m_data.end(); ++it)
            m_model->setNodeData(GraphModel::strToUuid(m_nodeId), it.key(), it.value());
        for (const auto &es : m_edges)
            m_model->connectPorts(GraphModel::strToUuid(es.sourceNode), es.sourcePort,
                                  GraphModel::strToUuid(es.targetNode), es.targetPort);
    }

    void redo() override {
        m_model->qmlRemoveNode(m_nodeId);
    }
};

// ── MoveNodeCommand ──────────────────────────────────────────

class MoveNodeCommand : public QUndoCommand {
    GraphModel *m_model;
    QString m_nodeId;
    QPointF m_oldPos;
    QPointF m_newPos;
public:
    MoveNodeCommand(GraphModel *model, const QString &nodeId,
                    const QPointF &oldPos, const QPointF &newPos)
        : m_model(model), m_nodeId(nodeId), m_oldPos(oldPos), m_newPos(newPos)
    {
        setText(QStringLiteral("Move node"));
    }

    void undo() override {
        m_model->qmlSetNodePosition(m_nodeId, m_oldPos.x(), m_oldPos.y());
    }

    void redo() override {
        m_model->qmlSetNodePosition(m_nodeId, m_newPos.x(), m_newPos.y());
    }

    int id() const override { return MOVE_NODE_ID; }

    bool mergeWith(const QUndoCommand *other) override {
        auto *cmd = dynamic_cast<const MoveNodeCommand *>(other);
        if (!cmd || cmd->m_nodeId != m_nodeId)
            return false;
        m_newPos = cmd->m_newPos;
        setText(QStringLiteral("Move node"));
        return true;
    }
};

// ── AddEdgeCommand ───────────────────────────────────────────

class AddEdgeCommand : public QUndoCommand {
    GraphModel *m_model;
    QString m_sourceNode, m_sourcePort, m_targetNode, m_targetPort;
    QString m_edgeId;
public:
    AddEdgeCommand(GraphModel *model,
                   const QString &sourceNode, const QString &sourcePort,
                   const QString &targetNode, const QString &targetPort)
        : m_model(model),
          m_sourceNode(sourceNode), m_sourcePort(sourcePort),
          m_targetNode(targetNode), m_targetPort(targetPort)
    {
        setText(QStringLiteral("Connect ports"));
    }

    void undo() override {
        if (!m_edgeId.isEmpty())
            m_model->qmlDisconnectEdge(m_edgeId);
    }

    void redo() override {
        QUuid id = m_model->connectPorts(GraphModel::strToUuid(m_sourceNode), m_sourcePort,
                                          GraphModel::strToUuid(m_targetNode), m_targetPort);
        m_edgeId = GraphModel::uuidToStr(id);
    }
};

// ── RemoveEdgeCommand ────────────────────────────────────────

class RemoveEdgeCommand : public QUndoCommand {
    GraphModel *m_model;
    QString m_edgeId;
    QString m_sourceNode, m_sourcePort, m_targetNode, m_targetPort;
public:
    RemoveEdgeCommand(GraphModel *model, const QString &edgeId)
        : m_model(model), m_edgeId(edgeId)
    {
        setText(QStringLiteral("Disconnect edge"));
        auto info = model->qmlEdgeInfo(edgeId);
        m_sourceNode = info.value("sourceNodeId").toString();
        m_sourcePort = info.value("sourcePort").toString();
        m_targetNode = info.value("targetNodeId").toString();
        m_targetPort = info.value("targetPort").toString();
    }

    void undo() override {
        m_model->connectPorts(GraphModel::strToUuid(m_sourceNode), m_sourcePort,
                              GraphModel::strToUuid(m_targetNode), m_targetPort);
    }

    void redo() override {
        m_model->qmlDisconnectEdge(m_edgeId);
    }
};

// ── SetNodeDataCommand ───────────────────────────────────────

class SetNodeDataCommand : public QUndoCommand {
    GraphModel *m_model;
    QString m_nodeId;
    QString m_key;
    QVariant m_oldValue;
    QVariant m_newValue;
public:
    SetNodeDataCommand(GraphModel *model, const QString &nodeId, const QString &key,
                       const QVariant &oldValue, const QVariant &newValue)
        : m_model(model), m_nodeId(nodeId), m_key(key),
          m_oldValue(oldValue), m_newValue(newValue)
    {
        setText(QStringLiteral("Edit %1").arg(key));
    }

    void undo() override {
        m_model->qmlSetNodeData(m_nodeId, m_key, m_oldValue);
    }

    void redo() override {
        m_model->qmlSetNodeData(m_nodeId, m_key, m_newValue);
    }

    int id() const override { return SET_DATA_ID; }

    bool mergeWith(const QUndoCommand *other) override {
        auto *cmd = dynamic_cast<const SetNodeDataCommand *>(other);
        if (!cmd || cmd->m_nodeId != m_nodeId || cmd->m_key != m_key)
            return false;
        m_newValue = cmd->m_newValue;
        setText(QStringLiteral("Edit %1").arg(m_key));
        return true;
    }
};

// ── UndoManager ──────────────────────────────────────────────

UndoManager::UndoManager(QObject *parent)
    : QObject(parent)
{
}

UndoManager::UndoManager(GraphModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
    ensureStack();
}

UndoManager::~UndoManager() = default;

GraphModel *UndoManager::graphModel() const { return m_model; }

void UndoManager::setGraphModel(GraphModel *model)
{
    if (m_model == model) return;
    m_model = model;
    ensureStack();
    emit graphModelChanged();
}

void UndoManager::ensureStack()
{
    if (m_stack || !m_model) return;
    m_stack = new QUndoStack(this);
    m_stack->setUndoLimit(100);
    connect(m_stack, &QUndoStack::canUndoChanged, this, &UndoManager::canUndoChanged);
    connect(m_stack, &QUndoStack::canRedoChanged, this, &UndoManager::canRedoChanged);
}

void UndoManager::undo() { if (m_stack) m_stack->undo(); }
void UndoManager::redo() { if (m_stack) m_stack->redo(); }
void UndoManager::clear() { if (m_stack) m_stack->clear(); }

bool UndoManager::canUndo() const { return m_stack && m_stack->canUndo(); }
bool UndoManager::canRedo() const { return m_stack && m_stack->canRedo(); }

QString UndoManager::qmlAddNode(const QString &type, double x, double y)
{
    auto *cmd = new AddNodeCommand(m_model, type, QPointF(x, y));
    m_stack->push(cmd);
    return cmd->nodeId();
}

void UndoManager::qmlRemoveNode(const QString &nodeId)
{
    m_stack->push(new RemoveNodeCommand(m_model, nodeId));
}

void UndoManager::qmlMoveNode(const QString &nodeId, double oldX, double oldY, double newX, double newY)
{
    m_stack->push(new MoveNodeCommand(m_model, nodeId, QPointF(oldX, oldY), QPointF(newX, newY)));
}

QString UndoManager::qmlConnectPorts(const QString &sourceNode, const QString &sourcePort,
                                      const QString &targetNode, const QString &targetPort)
{
    auto *cmd = new AddEdgeCommand(m_model, sourceNode, sourcePort, targetNode, targetPort);
    m_stack->push(cmd);
    return {}; // edge ID not needed by callers
}

void UndoManager::qmlDisconnectEdge(const QString &edgeId)
{
    m_stack->push(new RemoveEdgeCommand(m_model, edgeId));
}

void UndoManager::qmlSetNodeData(const QString &nodeId, const QString &key,
                                  const QVariant &oldValue, const QVariant &newValue)
{
    m_stack->push(new SetNodeDataCommand(m_model, nodeId, key, oldValue, newValue));
}

} // namespace NodeEditor
