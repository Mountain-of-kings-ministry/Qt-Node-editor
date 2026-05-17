#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QPointF>
#include <QMap>
#include <QList>
#include <QHash>
#include <QColor>
#include <QMutex>
#include <QVector>
#include <atomic>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace NodeEditor {

using NodeID = uint64_t;
using EdgeID = uint64_t;

inline QString idToStr(NodeID id) { return QString::number(id); }
inline NodeID strToId(const QString &str) { return str.toULongLong(); }

enum class PortType : uint8_t {
    Int, Float, String, Bool, Color, Generic,
    Double, Vec2, Vec3, Vec4, Array, Map, JSON, Image, AudioBuffer
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

// ── Layer A: Graph Topology (persistent) ──────────────────────
struct GraphNode {
    NodeID id = 0;
    QString type;
    QMap<QString, PortInfo> inputs;
    QMap<QString, PortInfo> outputs;
    uint32_t flags = 0;
};

// ── Layer A: Edges ────────────────────────────────────────────
struct GraphEdge {
    EdgeID id = 0;
    NodeID sourceNodeId = 0;
    QString sourcePort;
    NodeID targetNodeId = 0;
    QString targetPort;
};

// ── Layer B: Runtime Cache (transient) ────────────────────────
struct NodeRuntime {
    QVariantMap data;          // port values (QML-compatible)
    uint64_t evalVersion = 0;   // bumped on each compute; read by downstream nodes
};

// ── Layer C: UI State (editor only) ───────────────────────────
struct NodeUIState {
    double x = 0, y = 0;
    bool selected : 1 = false;
    bool collapsed : 1 = false;
    float previewZoom = 1.0f;
};

// ── Topology Cache ────────────────────────────────────────────
struct TopologyCache {
    QList<NodeID> sorted;
    QHash<NodeID, QList<NodeID>> adjacency;
    QHash<NodeID, int> inDegree;
    uint64_t edgeVersion = 0;
    bool valid = false;
    void invalidate() { valid = false; }
};

class GraphModel : public QObject {
    Q_OBJECT
public:
    explicit GraphModel(QObject *parent = nullptr);
    ~GraphModel() override;

    // ── Internal C++ API (NodeID / EdgeID) ──────────────────
    NodeID addNode(const QString &type, const QPointF &position = QPointF(0, 0),
                   NodeID existingId = 0);
    void removeNode(NodeID nodeId);

          GraphNode *graphNode(NodeID nodeId);
    const GraphNode *graphNode(NodeID nodeId) const;
          NodeRuntime *nodeRuntime(NodeID nodeId);
    const NodeRuntime *nodeRuntime(NodeID nodeId) const;
          NodeUIState *nodeUIState(NodeID nodeId);
    const NodeUIState *nodeUIState(NodeID nodeId) const;

    const QHash<NodeID, GraphNode> &allGraphNodes() const { return m_graphNodes; }
    const QHash<NodeID, GraphEdge> &allGraphEdges() const { return m_graphEdges; }

    EdgeID connectPorts(NodeID sourceNode, const QString &sourcePort,
                        NodeID targetNode, const QString &targetPort);
    void disconnectEdge(EdgeID edgeId);
    const GraphEdge *graphEdge(EdgeID edgeId) const;

    void setNodeData(NodeID nodeId, const QString &key, const QVariant &value);
    QVariant nodeData(NodeID nodeId, const QString &key) const;

    void setNodePosition(NodeID nodeId, const QPointF &position);
    QPointF nodePosition(NodeID nodeId) const;

    const QList<NodeID> &cachedTopologicalOrder();
    // Returns nodes grouped by topological depth (parallel-ready levels)
    QVector<QList<NodeID>> topologicalLevels();
    bool hasCycles();

    // ── Registry ─────────────────────────────────────────────
    void registerNodeType(const QString &type, const NodeTypeInfo &info);
    const NodeTypeInfo *nodeTypeInfo(const QString &type) const;
    void registerCategory(const NodeCategory &cat);
    Q_INVOKABLE QVariantList qmlCategories() const;
    Q_INVOKABLE QStringList qmlNodesInCategory(const QString &categoryId) const;
    Q_INVOKABLE QStringList qmlAllNodeTypes() const;

    // ── Serialization (RapidJSON) ────────────────────────────
    Q_INVOKABLE QString qmlSerializeToJson() const;
    Q_INVOKABLE void qmlDeserializeFromJson(const QString &json);
    Q_INVOKABLE bool qmlSaveToFile(const QString &path);
    Q_INVOKABLE bool qmlLoadFromFile(const QString &path);
    Q_INVOKABLE void clear();

    // ── Multi-tab ────────────────────────────────────────────
    Q_INVOKABLE void qmlCopyRegistryFrom(GraphModel *source);

    // ── QML-friendly API ─────────────────────────────────────
    Q_INVOKABLE QString qmlAddNode(const QString &type, double x, double y);
    Q_INVOKABLE void qmlRemoveNode(const QString &nodeId);
    Q_INVOKABLE QStringList qmlNodeIds() const;
    Q_INVOKABLE QVariantMap qmlNodeInfo(const QString &nodeId) const;
    Q_INVOKABLE QPointF qmlNodePosition(const QString &nodeId) const;
    Q_INVOKABLE void qmlSetNodePosition(const QString &nodeId, double x, double y);
    Q_INVOKABLE void qmlSetNodeData(const QString &nodeId, const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant qmlNodeData(const QString &nodeId, const QString &key) const;
    Q_INVOKABLE QString qmlConnectPorts(const QString &sourceNode, const QString &sourcePort,
                                        const QString &targetNode, const QString &targetPort);
    Q_INVOKABLE void qmlDisconnectEdge(const QString &edgeId);
    Q_INVOKABLE void qmlDisconnectPort(const QString &nodeId, const QString &portName, bool isInput);
    Q_INVOKABLE QStringList qmlEdgeIds() const;
    Q_INVOKABLE QVariantMap qmlEdgeInfo(const QString &edgeId) const;
    Q_INVOKABLE QStringList qmlNodeInputPorts(const QString &nodeId) const;
    Q_INVOKABLE QStringList qmlNodeOutputPorts(const QString &nodeId) const;
    Q_INVOKABLE int qmlPortType(const QString &nodeId, const QString &port, bool isInput) const;
    Q_INVOKABLE bool qmlIsPortConnected(const QString &nodeId, const QString &port, bool isInput) const;

    // ── Dynamic ports ────────────────────────────────────────
    Q_INVOKABLE void qmlAddInputPort(const QString &nodeId, const QString &portName, int portType);
    Q_INVOKABLE void qmlRemoveInputPort(const QString &nodeId, const QString &portName);
    Q_INVOKABLE void qmlAddOutputPort(const QString &nodeId, const QString &portName, int portType);
    Q_INVOKABLE void qmlRemoveOutputPort(const QString &nodeId, const QString &portName);
    Q_INVOKABLE void qmlLoadCanvasFile(const QString &nodeId, const QString &filePath);

    // ── Static helpers ───────────────────────────────────────
    static int portTypeToInt(PortType t);
    Q_INVOKABLE static QString portTypeColor(int portType);
    static QString portTypeName(int portType);
    Q_INVOKABLE static QString qmlScreenColorAt(int x, int y);

    // ── Public ID helpers (for UndoManager) ──────────────────
    static QString nodeIdToStr(NodeID id);
    static NodeID strToNodeId(const QString &str);

signals:
    void nodeAdded(NodeID nodeId);
    void nodeRemoved(NodeID nodeId);
    void edgeAdded(EdgeID edgeId);
    void edgeRemoved(EdgeID edgeId);
    void nodeDataChanged(NodeID nodeId, QString key);
    void nodePositionChanged(NodeID nodeId);
    void nodesDirty(QList<uint64_t> nodeIds);

    void qmlNodeAdded(QString nodeId);
    void qmlNodeRemoved(QString nodeId);
    void qmlEdgeAdded(QString edgeId);
    void qmlEdgeRemoved(QString edgeId);
    void qmlNodeDataChanged(QString nodeId, QString key);
    void qmlNodePositionChanged(QString nodeId);
    void qmlNodePortsChanged(QString nodeId);

private:
    // ── Core 3-layer storage ────────────────────────────────
    QHash<NodeID, GraphNode> m_graphNodes;
    QHash<NodeID, GraphEdge> m_graphEdges;
    QHash<NodeID, NodeRuntime> m_runtimes;
    QHash<NodeID, NodeUIState> m_uiStates;

    // ── Registry data ───────────────────────────────────────
    QHash<QString, NodeTypeInfo> m_nodeTypes;
    QList<NodeCategory> m_categories;

    // ── Topology cache ──────────────────────────────────────
    TopologyCache m_topology;

    // ── Thread safety ───────────────────────────────────────
    mutable QMutex m_runtimeMutex;

    // ── ID counter ──────────────────────────────────────────
    std::atomic<uint64_t> m_nextId{1};

    // ── Internal JSON helpers (RapidJSON for serialization) ──
    QVariant jsonValToQVariant(const rapidjson::Value &val) const;
    rapidjson::Value qvariantToJsonVal(const QVariant &qv,
                                       rapidjson::Document::AllocatorType &alloc) const;
};

} // namespace NodeEditor
