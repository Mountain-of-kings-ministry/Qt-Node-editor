#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QPointF>
#include <QUuid>
#include <QMap>
#include <QList>
#include <QHash>
#include <QColor>

namespace NodeEditor {

enum class PortType {
    Int,
    Float,
    String,
    Bool,
    Color,
    Generic,
    Double,
    Vec2,
    Vec3,
    Vec4,
    Array,
    Map,
    JSON,
    Image,
    AudioBuffer
};

struct PortInfo {
    PortType type = PortType::Generic;
    QString name;
    QVariant defaultValue;
};

struct NodeCategory {
    QString id;
    QString displayName;
    QColor color;
};

struct NodeTypeInfo {
    QMap<QString, PortInfo> inputs;
    QMap<QString, PortInfo> outputs;
    QString displayColor = "#4A9EFF";
    QString categoryId;
    QString subCategory;
    QString nodeName;
};

struct NodeData {
    QUuid id;
    QString type;
    QPointF position;
    QMap<QString, PortInfo> inputs;
    QMap<QString, PortInfo> outputs;
    QVariantMap data;
};

struct EdgeData {
    QUuid id;
    QUuid sourceNodeId;
    QString sourcePort;
    QUuid targetNodeId;
    QString targetPort;
};

class GraphModel : public QObject {
    Q_OBJECT
public:
    explicit GraphModel(QObject *parent = nullptr);

    // Internal C++ API (uses QUuid)
    QUuid addNode(const QString &type, const QPointF &position = QPointF(0, 0),
                  const QUuid &existingId = QUuid());
    void removeNode(const QUuid &nodeId);
    NodeData *node(const QUuid &nodeId);
    const QList<NodeData> &nodes() const;

    QUuid connectPorts(const QUuid &sourceNode, const QString &sourcePort,
                       const QUuid &targetNode, const QString &targetPort);
    void disconnectEdge(const QUuid &edgeId);
    const QList<EdgeData> &edges() const;

    void setNodeData(const QUuid &nodeId, const QString &key, const QVariant &value);
    QVariant nodeData(const QUuid &nodeId, const QString &key) const;

    void setNodePosition(const QUuid &nodeId, const QPointF &position);
    QPointF nodePosition(const QUuid &nodeId) const;

    QList<QUuid> topologicalSort() const;
    bool hasCycles() const;

    // Node type registry
    void registerNodeType(const QString &type, const NodeTypeInfo &info);
    const NodeTypeInfo *nodeTypeInfo(const QString &type) const;

    // Category registry
    void registerCategory(const NodeCategory &cat);
    Q_INVOKABLE QVariantList qmlCategories() const;
    Q_INVOKABLE QStringList qmlNodesInCategory(const QString &categoryId) const;
    Q_INVOKABLE QStringList qmlAllNodeTypes() const;

    // Serialization
    Q_INVOKABLE QString qmlSerializeToJson() const;
    Q_INVOKABLE void qmlDeserializeFromJson(const QString &json);
    Q_INVOKABLE bool qmlSaveToFile(const QString &path);
    Q_INVOKABLE bool qmlLoadFromFile(const QString &path);
    Q_INVOKABLE void clear();

    // Copy registrations from another model (for multi-tab support)
    Q_INVOKABLE void qmlCopyRegistryFrom(GraphModel *source);

    // QML-friendly API (uses QString for IDs)
    Q_INVOKABLE QString qmlAddNode(const QString &type, double x, double y);
    Q_INVOKABLE void qmlRemoveNode(const QString &nodeId);
    Q_INVOKABLE QStringList qmlNodeIds() const;
    Q_INVOKABLE QVariantMap qmlNodeInfo(const QString &nodeId) const;
    Q_INVOKABLE void qmlSetNodePosition(const QString &nodeId, double x, double y);
    Q_INVOKABLE void qmlSetNodeData(const QString &nodeId, const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant qmlNodeData(const QString &nodeId, const QString &key) const;
    Q_INVOKABLE QString qmlConnectPorts(const QString &sourceNode, const QString &sourcePort,
                                        const QString &targetNode, const QString &targetPort);
    Q_INVOKABLE void qmlDisconnectEdge(const QString &edgeId);
    Q_INVOKABLE QStringList qmlEdgeIds() const;
    Q_INVOKABLE QVariantMap qmlEdgeInfo(const QString &edgeId) const;
    Q_INVOKABLE QStringList qmlNodeInputPorts(const QString &nodeId) const;
    Q_INVOKABLE QStringList qmlNodeOutputPorts(const QString &nodeId) const;
    Q_INVOKABLE int qmlPortType(const QString &nodeId, const QString &port, bool isInput) const;
    Q_INVOKABLE bool qmlIsPortConnected(const QString &nodeId, const QString &port, bool isInput) const;

    // Dynamic port management
    Q_INVOKABLE void qmlAddInputPort(const QString &nodeId, const QString &portName, int portType);
    Q_INVOKABLE void qmlRemoveInputPort(const QString &nodeId, const QString &portName);
    Q_INVOKABLE void qmlAddOutputPort(const QString &nodeId, const QString &portName, int portType);
    Q_INVOKABLE void qmlRemoveOutputPort(const QString &nodeId, const QString &portName);
    // Load a canvas sub-graph file and dynamically create ports on the target node
    Q_INVOKABLE void qmlLoadCanvasFile(const QString &nodeId, const QString &filePath);

    // Static helpers
    static int portTypeToInt(PortType t);
    static QString portTypeColor(int portType);
    static QString portTypeName(int portType);

    // ID conversion (public for UndoManager use)
    static QString uuidToStr(const QUuid &id);
    static QUuid strToUuid(const QString &str);

signals:
    // C++ signals (QUuid)
    void nodeAdded(QUuid nodeId);
    void nodeRemoved(QUuid nodeId);
    void edgeAdded(QUuid edgeId);
    void edgeRemoved(QUuid edgeId);
    void nodeDataChanged(QUuid nodeId, QString key);
    void nodePositionChanged(QUuid nodeId);
    void nodesDirty(QList<QUuid> nodeIds);

    // QML-friendly signals (QString)
    void qmlNodeAdded(QString nodeId);
    void qmlNodeRemoved(QString nodeId);
    void qmlEdgeAdded(QString edgeId);
    void qmlEdgeRemoved(QString edgeId);
    void qmlNodeDataChanged(QString nodeId, QString key);
    void qmlNodePositionChanged(QString nodeId);
    void qmlNodePortsChanged(QString nodeId);

private:
    QList<NodeData> m_nodes;
    QList<EdgeData> m_edges;
    QHash<QString, NodeTypeInfo> m_nodeTypes;
    QList<NodeCategory> m_categories;
};

} // namespace NodeEditor
