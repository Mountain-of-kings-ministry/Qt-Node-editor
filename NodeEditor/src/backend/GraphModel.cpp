#include "NodeEditor/GraphModel.h"
#include <QFile>
#include <QSet>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

namespace NodeEditor {

// ══════════════════════════════════════════════════════════════
// Construction / Destruction
// ══════════════════════════════════════════════════════════════

GraphModel::GraphModel(QObject *parent) : QObject(parent) {}
GraphModel::~GraphModel() = default;

QString GraphModel::nodeIdToStr(NodeID id) { return ::NodeEditor::idToStr(id); }
NodeID GraphModel::strToNodeId(const QString &str) { return ::NodeEditor::strToId(str); }

// ══════════════════════════════════════════════════════════════
// JSON ↔ QVariant converters (for RapidJSON serialization)
// ══════════════════════════════════════════════════════════════

QVariant GraphModel::jsonValToQVariant(const rapidjson::Value &val) const
{
    switch (val.GetType()) {
    case rapidjson::kNullType:   return {};
    case rapidjson::kFalseType:  return false;
    case rapidjson::kTrueType:   return true;
    case rapidjson::kNumberType:
        if (val.IsInt64())   return (qlonglong)val.GetInt64();
        if (val.IsUint64())  return (qulonglong)val.GetUint64();
        if (val.IsDouble())  return val.GetDouble();
        return val.GetInt();
    case rapidjson::kStringType:
        return QString::fromUtf8(val.GetString(), (int)val.GetStringLength());
    case rapidjson::kArrayType: {
        QVariantList list;
        list.reserve((int)val.Size());
        for (const auto &v : val.GetArray())
            list.append(jsonValToQVariant(v));
        return list;
    }
    case rapidjson::kObjectType: {
        QVariantMap map;
        for (const auto &m : val.GetObject())
            map[QString::fromUtf8(m.name.GetString(), (int)m.name.GetStringLength())]
                = jsonValToQVariant(m.value);
        return map;
    }
    }
    return {};
}

rapidjson::Value GraphModel::qvariantToJsonVal(
    const QVariant &qv, rapidjson::Document::AllocatorType &alloc) const
{
    if (qv.isNull())
        return rapidjson::Value(rapidjson::kNullType);

    switch (qv.typeId()) {
    case QMetaType::Bool:
        return rapidjson::Value(qv.toBool());
    case QMetaType::Int:
    case QMetaType::Long:
    case QMetaType::LongLong:
        return rapidjson::Value((int64_t)qv.toLongLong());
    case QMetaType::UInt:
        return rapidjson::Value(qv.toUInt());
    case QMetaType::ULongLong:
        return rapidjson::Value((uint64_t)qv.toULongLong());
    case QMetaType::Double:
    case QMetaType::Float:
        return rapidjson::Value(qv.toDouble());
    case QMetaType::QString: {
        const QByteArray u = qv.toString().toUtf8();
        rapidjson::Value s;
        s.SetString(u.constData(), (rapidjson::SizeType)u.size(), alloc);
        return s;
    }
    case QMetaType::QVariantList:
    case QMetaType::QStringList: {
        rapidjson::Value arr(rapidjson::kArrayType);
        for (const auto &v : qv.toList())
            arr.PushBack(qvariantToJsonVal(v, alloc), alloc);
        return arr;
    }
    case QMetaType::QVariantMap: {
        rapidjson::Value obj(rapidjson::kObjectType);
        const auto map = qv.toMap();
        for (auto it = map.begin(); it != map.end(); ++it) {
            const QByteArray k = it.key().toUtf8();
            obj.AddMember(
                rapidjson::Value(k.constData(), (rapidjson::SizeType)k.size(), alloc),
                qvariantToJsonVal(it.value(), alloc), alloc);
        }
        return obj;
    }
    default: {
        const QByteArray u = qv.toString().toUtf8();
        rapidjson::Value s;
        s.SetString(u.constData(), (rapidjson::SizeType)u.size(), alloc);
        return s;
    }
    }
}

// ══════════════════════════════════════════════════════════════
// addNode
// ══════════════════════════════════════════════════════════════

NodeID GraphModel::addNode(const QString &type, const QPointF &position, NodeID existingId)
{
    NodeID id = existingId != 0 ? existingId : m_nextId.fetch_add(1, std::memory_order_relaxed);

    GraphNode gn;
    gn.id = id;
    gn.type = type;

    NodeRuntime rt;
    rt.evalVersion = 0;

    NodeUIState ui;
    ui.x = position.x();
    ui.y = position.y();

    if (auto *info = nodeTypeInfo(type)) {
        for (auto it = info->inputs.begin(); it != info->inputs.end(); ++it) {
            gn.inputs[it.key()] = it.value();
            if (it.value().defaultValue.isValid())
                rt.data[it.key()] = it.value().defaultValue;
        }
        for (auto it = info->outputs.begin(); it != info->outputs.end(); ++it) {
            gn.outputs[it.key()] = it.value();
            if (it.value().defaultValue.isValid())
                rt.data[it.key()] = it.value().defaultValue;
        }
    }

    m_graphNodes.insert(id, std::move(gn));
    m_runtimes.insert(id, std::move(rt));
    m_uiStates.insert(id, std::move(ui));

    emit nodeAdded(id);
    emit qmlNodeAdded(idToStr(id));
    return id;
}

// ══════════════════════════════════════════════════════════════
// removeNode
// ══════════════════════════════════════════════════════════════

void GraphModel::removeNode(NodeID nodeId)
{
    const QString idStr = idToStr(nodeId);

    QList<EdgeID> removedEdges;
    QList<QPair<NodeID, QString>> downstreamResets;
    for (const auto &e : std::as_const(m_graphEdges)) {
        if (e.sourceNodeId == nodeId)
            downstreamResets.append({e.targetNodeId, e.targetPort});
        if (e.sourceNodeId == nodeId || e.targetNodeId == nodeId)
            removedEdges.append(e.id);
    }

    for (EdgeID eid : removedEdges) {
        m_graphEdges.remove(eid);
        emit edgeRemoved(eid);
        emit qmlEdgeRemoved(idToStr(eid));
    }
    m_topology.invalidate();

    for (const auto &[tgtId, tgtPort] : downstreamResets) {
        if (auto *n = graphNode(tgtId)) {
            if (auto *typeInfo = nodeTypeInfo(n->type)) {
                if (typeInfo->inputs.contains(tgtPort))
                    setNodeData(tgtId, tgtPort, typeInfo->inputs[tgtPort].defaultValue);
            }
        }
    }

    m_graphNodes.remove(nodeId);
    m_runtimes.remove(nodeId);
    m_uiStates.remove(nodeId);

    emit nodeRemoved(nodeId);
    emit qmlNodeRemoved(idStr);
}

// ══════════════════════════════════════════════════════════════
// Node accessors
// ══════════════════════════════════════════════════════════════

GraphNode *GraphModel::graphNode(NodeID nodeId)
{
    auto it = m_graphNodes.find(nodeId);
    return it != m_graphNodes.end() ? &it.value() : nullptr;
}

const GraphNode *GraphModel::graphNode(NodeID nodeId) const
{
    auto it = m_graphNodes.find(nodeId);
    return it != m_graphNodes.end() ? &it.value() : nullptr;
}

NodeRuntime *GraphModel::nodeRuntime(NodeID nodeId)
{
    auto it = m_runtimes.find(nodeId);
    return it != m_runtimes.end() ? &it.value() : nullptr;
}

const NodeRuntime *GraphModel::nodeRuntime(NodeID nodeId) const
{
    auto it = m_runtimes.find(nodeId);
    return it != m_runtimes.end() ? &it.value() : nullptr;
}

NodeUIState *GraphModel::nodeUIState(NodeID nodeId)
{
    auto it = m_uiStates.find(nodeId);
    return it != m_uiStates.end() ? &it.value() : nullptr;
}

const NodeUIState *GraphModel::nodeUIState(NodeID nodeId) const
{
    auto it = m_uiStates.find(nodeId);
    return it != m_uiStates.end() ? &it.value() : nullptr;
}

// ══════════════════════════════════════════════════════════════
// Edge management
// ══════════════════════════════════════════════════════════════

EdgeID GraphModel::connectPorts(NodeID sourceNode, const QString &sourcePort,
                                NodeID targetNode, const QString &targetPort)
{
    EdgeID id = m_nextId.fetch_add(1, std::memory_order_relaxed);
    GraphEdge e;
    e.id = id;
    e.sourceNodeId = sourceNode;
    e.sourcePort = sourcePort;
    e.targetNodeId = targetNode;
    e.targetPort = targetPort;
    m_graphEdges.insert(id, std::move(e));
    m_topology.invalidate();
    emit edgeAdded(id);
    emit qmlEdgeAdded(idToStr(id));
    return id;
}

void GraphModel::disconnectEdge(EdgeID edgeId)
{
    auto it = m_graphEdges.find(edgeId);
    if (it == m_graphEdges.end()) return;

    NodeID targetNodeId = it.value().targetNodeId;
    QString targetPort = it.value().targetPort;
    QString idStr = idToStr(edgeId);

    m_graphEdges.erase(it);
    m_topology.invalidate();

    if (targetNodeId != 0) {
        if (auto *n = graphNode(targetNodeId)) {
            if (auto *typeInfo = nodeTypeInfo(n->type)) {
                if (typeInfo->inputs.contains(targetPort))
                    setNodeData(targetNodeId, targetPort,
                                typeInfo->inputs[targetPort].defaultValue);
            }
        }
    }

    emit edgeRemoved(edgeId);
    emit qmlEdgeRemoved(idStr);
}

const GraphEdge *GraphModel::graphEdge(EdgeID edgeId) const
{
    auto it = m_graphEdges.find(edgeId);
    return it != m_graphEdges.end() ? &it.value() : nullptr;
}

// ══════════════════════════════════════════════════════════════
// Runtime data
// ══════════════════════════════════════════════════════════════

void GraphModel::setNodeData(NodeID nodeId, const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_runtimeMutex);
    auto *rt = nodeRuntime(nodeId);
    if (!rt) return;
    rt->data[key] = value;
    lock.unlock();
    emit nodeDataChanged(nodeId, key);
    emit qmlNodeDataChanged(idToStr(nodeId), key);
}

QVariant GraphModel::nodeData(NodeID nodeId, const QString &key) const
{
    QMutexLocker lock(&m_runtimeMutex);
    const auto *rt = nodeRuntime(nodeId);
    if (!rt) return {};
    auto it = rt->data.find(key);
    return it != rt->data.end() ? it.value() : QVariant();
}

void GraphModel::setNodePosition(NodeID nodeId, const QPointF &position)
{
    auto *ui = nodeUIState(nodeId);
    if (!ui) return;
    ui->x = position.x();
    ui->y = position.y();
    emit nodePositionChanged(nodeId);
    emit qmlNodePositionChanged(idToStr(nodeId));
}

QPointF GraphModel::nodePosition(NodeID nodeId) const
{
    const auto *ui = nodeUIState(nodeId);
    if (!ui) return {};
    return QPointF(ui->x, ui->y);
}

// ══════════════════════════════════════════════════════════════
// Topology
// ══════════════════════════════════════════════════════════════

const QList<NodeID> &GraphModel::cachedTopologicalOrder()
{
    if (m_topology.valid)
        return m_topology.sorted;

    m_topology.adjacency.clear();
    m_topology.inDegree.clear();
    m_topology.sorted.clear();

    for (const auto &n : m_graphNodes) {
        m_topology.inDegree[n.id] = 0;
        m_topology.adjacency[n.id] = {};
    }

    for (const auto &e : m_graphEdges) {
        m_topology.adjacency[e.sourceNodeId].append(e.targetNodeId);
        m_topology.inDegree[e.targetNodeId]++;
    }

    QList<NodeID> queue;
    for (auto it = m_topology.inDegree.begin(); it != m_topology.inDegree.end(); ++it)
        if (it.value() == 0)
            queue.append(it.key());

    while (!queue.isEmpty()) {
        NodeID id = queue.takeFirst();
        m_topology.sorted.append(id);
        for (NodeID neighbor : m_topology.adjacency[id]) {
            m_topology.inDegree[neighbor]--;
            if (m_topology.inDegree[neighbor] == 0)
                queue.append(neighbor);
        }
    }

    m_topology.valid = true;
    return m_topology.sorted;
}

bool GraphModel::hasCycles()
{
    return cachedTopologicalOrder().size() != m_graphNodes.size();
}

QVector<QList<NodeID>> GraphModel::topologicalLevels()
{
    cachedTopologicalOrder(); // ensure topology is valid
    if (!m_topology.valid) return {};

    // BFS to compute depth of each node
    QHash<NodeID, int> depth;
    for (NodeID id : m_topology.sorted)
        depth[id] = 0;

    for (NodeID id : m_topology.sorted) {
        int d = depth[id];
        for (NodeID neighbor : m_topology.adjacency[id]) {
            if (depth[neighbor] < d + 1)
                depth[neighbor] = d + 1;
        }
    }

    // Group by depth
    int maxDepth = 0;
    for (auto it = depth.begin(); it != depth.end(); ++it)
        if (it.value() > maxDepth) maxDepth = it.value();

    QVector<QList<NodeID>> levels(maxDepth + 1);
    for (auto it = depth.begin(); it != depth.end(); ++it)
        levels[it.value()].append(it.key());

    return levels;
}

// ══════════════════════════════════════════════════════════════
// Registry
// ══════════════════════════════════════════════════════════════

void GraphModel::registerNodeType(const QString &type, const NodeTypeInfo &info)
{
    m_nodeTypes[type] = info;
}

const NodeTypeInfo *GraphModel::nodeTypeInfo(const QString &type) const
{
    auto it = m_nodeTypes.find(type);
    return it != m_nodeTypes.end() ? &it.value() : nullptr;
}

void GraphModel::registerCategory(const NodeCategory &cat)
{
    for (auto &c : m_categories) {
        if (c.id == cat.id) { c = cat; return; }
    }
    m_categories.append(cat);
}

QVariantList GraphModel::qmlCategories() const
{
    QVariantList result;
    for (const auto &c : m_categories) {
        QVariantMap m;
        m["id"] = c.id;
        m["displayName"] = c.displayName;
        m["color"] = c.color.name();
        result.append(m);
    }
    return result;
}

QStringList GraphModel::qmlNodesInCategory(const QString &categoryId) const
{
    QStringList result;
    for (auto it = m_nodeTypes.begin(); it != m_nodeTypes.end(); ++it)
        if (it.value().categoryId == categoryId)
            result.append(it.key());
    return result;
}

QStringList GraphModel::qmlAllNodeTypes() const
{
    QStringList result;
    QSet<QString> seen;
    for (const auto &cat : m_categories) {
        for (auto it = m_nodeTypes.begin(); it != m_nodeTypes.end(); ++it)
            if (it.value().categoryId == cat.id) { result.append(it.key()); seen.insert(it.key()); }
    }
    for (auto it = m_nodeTypes.begin(); it != m_nodeTypes.end(); ++it)
        if (!seen.contains(it.key()))
            result.append(it.key());
    return result;
}

// ══════════════════════════════════════════════════════════════
// QML-friendly API
// ══════════════════════════════════════════════════════════════

QString GraphModel::qmlAddNode(const QString &type, double x, double y)
{
    return idToStr(addNode(type, QPointF(x, y)));
}

void GraphModel::qmlRemoveNode(const QString &nodeId)
{
    removeNode(strToId(nodeId));
}

QStringList GraphModel::qmlNodeIds() const
{
    QStringList ids; ids.reserve(m_graphNodes.size());
    for (const auto &n : m_graphNodes)
        ids.append(idToStr(n.id));
    return ids;
}

QVariantMap GraphModel::qmlNodeInfo(const QString &nodeIdStr) const
{
    NodeID id = strToId(nodeIdStr);
    const auto *gn = graphNode(id);
    if (!gn) return {};
    const auto *ui = nodeUIState(id);
    const auto *typeInfo = nodeTypeInfo(gn->type);

    QVariantMap info;
    info["id"] = idToStr(gn->id);
    info["type"] = gn->type;
    info["x"] = ui ? ui->x : 0.0;
    info["y"] = ui ? ui->y : 0.0;

    QStringList inPorts, outPorts;
    QVariantList inPortTypes, outPortTypes;
    for (auto it = gn->inputs.begin(); it != gn->inputs.end(); ++it) {
        inPorts.append(it.key());
        inPortTypes.append(static_cast<int>(it.value().type));
    }
    for (auto it = gn->outputs.begin(); it != gn->outputs.end(); ++it) {
        outPorts.append(it.key());
        outPortTypes.append(static_cast<int>(it.value().type));
    }
    info["inputPorts"] = inPorts;
    info["inputPortTypes"] = inPortTypes;
    info["outputPorts"] = outPorts;
    info["outputPortTypes"] = outPortTypes;
    info["color"] = typeInfo ? typeInfo->displayColor : "#4A9EFF";
    info["categoryId"] = typeInfo ? typeInfo->categoryId : QString();
    info["subCategory"] = typeInfo ? typeInfo->subCategory : QString();
    info["nodeName"] = typeInfo ? typeInfo->nodeName : QString();
    return info;
}

QPointF GraphModel::qmlNodePosition(const QString &nodeId) const { return nodePosition(strToId(nodeId)); }
void GraphModel::qmlSetNodePosition(const QString &nodeId, double x, double y) { setNodePosition(strToId(nodeId), QPointF(x, y)); }
void GraphModel::qmlSetNodeData(const QString &nodeId, const QString &key, const QVariant &value) { setNodeData(strToId(nodeId), key, value); }
QVariant GraphModel::qmlNodeData(const QString &nodeId, const QString &key) const { return nodeData(strToId(nodeId), key); }

QString GraphModel::qmlConnectPorts(const QString &sourceNode, const QString &sourcePort,
                                    const QString &targetNode, const QString &targetPort)
{
    return idToStr(connectPorts(strToId(sourceNode), sourcePort, strToId(targetNode), targetPort));
}

void GraphModel::qmlDisconnectEdge(const QString &edgeId) { disconnectEdge(strToId(edgeId)); }

void GraphModel::qmlDisconnectPort(const QString &nodeId, const QString &portName, bool isInput)
{
    NodeID id = strToId(nodeId);
    QList<EdgeID> toRemove;
    for (const auto &e : std::as_const(m_graphEdges))
        if (isInput ? (e.targetNodeId == id && e.targetPort == portName)
                    : (e.sourceNodeId == id && e.sourcePort == portName))
            toRemove.append(e.id);
    for (EdgeID eid : toRemove) disconnectEdge(eid);
}

QStringList GraphModel::qmlEdgeIds() const
{
    QStringList ids; ids.reserve(m_graphEdges.size());
    for (const auto &e : m_graphEdges)
        ids.append(idToStr(e.id));
    return ids;
}

QVariantMap GraphModel::qmlEdgeInfo(const QString &edgeId) const
{
    const auto *e = graphEdge(strToId(edgeId));
    if (!e) return {};
    QVariantMap info;
    info["id"] = idToStr(e->id);
    info["sourceNodeId"] = idToStr(e->sourceNodeId);
    info["sourcePort"] = e->sourcePort;
    info["targetNodeId"] = idToStr(e->targetNodeId);
    info["targetPort"] = e->targetPort;
    return info;
}

QStringList GraphModel::qmlNodeInputPorts(const QString &nodeId) const
{
    const auto *gn = graphNode(strToId(nodeId));
    if (!gn) return {};
    QStringList ports;
    for (auto it = gn->inputs.begin(); it != gn->inputs.end(); ++it) ports.append(it.key());
    return ports;
}

QStringList GraphModel::qmlNodeOutputPorts(const QString &nodeId) const
{
    const auto *gn = graphNode(strToId(nodeId));
    if (!gn) return {};
    QStringList ports;
    for (auto it = gn->outputs.begin(); it != gn->outputs.end(); ++it) ports.append(it.key());
    return ports;
}

int GraphModel::qmlPortType(const QString &nodeId, const QString &port, bool isInput) const
{
    const auto *gn = graphNode(strToId(nodeId));
    if (!gn) return static_cast<int>(PortType::Generic);
    if (isInput) {
        auto it = gn->inputs.find(port);
        if (it != gn->inputs.end()) return static_cast<int>(it.value().type);
    } else {
        auto it = gn->outputs.find(port);
        if (it != gn->outputs.end()) return static_cast<int>(it.value().type);
    }
    return static_cast<int>(PortType::Generic);
}

bool GraphModel::qmlIsPortConnected(const QString &nodeId, const QString &port, bool isInput) const
{
    NodeID id = strToId(nodeId);
    for (const auto &e : std::as_const(m_graphEdges))
        if (isInput ? (e.targetNodeId == id && e.targetPort == port)
                    : (e.sourceNodeId == id && e.sourcePort == port))
            return true;
    return false;
}

// ══════════════════════════════════════════════════════════════
// Dynamic port management
// ══════════════════════════════════════════════════════════════

void GraphModel::qmlAddInputPort(const QString &nodeId, const QString &portName, int portType)
{
    auto *gn = graphNode(strToId(nodeId));
    if (!gn || gn->inputs.contains(portName)) return;
    gn->inputs[portName] = PortInfo{static_cast<PortType>(portType), portName, QVariant()};
    emit qmlNodePortsChanged(nodeId);
    emit nodeDataChanged(strToId(nodeId), portName);
}

void GraphModel::qmlRemoveInputPort(const QString &nodeId, const QString &portName)
{
    auto *gn = graphNode(strToId(nodeId));
    if (!gn) return;
    gn->inputs.remove(portName);
    if (auto *rt = nodeRuntime(strToId(nodeId)))
        rt->data.remove(portName);
    emit qmlNodePortsChanged(nodeId);
}

void GraphModel::qmlAddOutputPort(const QString &nodeId, const QString &portName, int portType)
{
    auto *gn = graphNode(strToId(nodeId));
    if (!gn || gn->outputs.contains(portName)) return;
    gn->outputs[portName] = PortInfo{static_cast<PortType>(portType), portName, QVariant()};
    emit qmlNodePortsChanged(nodeId);
    emit nodeDataChanged(strToId(nodeId), portName);
}

void GraphModel::qmlRemoveOutputPort(const QString &nodeId, const QString &portName)
{
    auto *gn = graphNode(strToId(nodeId));
    if (!gn) return;
    gn->outputs.remove(portName);
    if (auto *rt = nodeRuntime(strToId(nodeId)))
        rt->data.remove(portName);
    emit qmlNodePortsChanged(nodeId);
}

void GraphModel::qmlLoadCanvasFile(const QString &nodeId, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) { qWarning() << "qmlLoadCanvasFile: cannot open file:" << filePath; return; }
    QByteArray raw = file.readAll(); file.close();

    rapidjson::Document doc;
    doc.Parse(raw.constData());
    if (doc.HasParseError()) { qWarning() << "JSON parse error:" << rapidjson::GetParseError_En(doc.GetParseError()); return; }
    if (!doc.IsObject()) return;

    auto &nodesArr = doc["nodes"];
    if (!nodesArr.IsArray()) return;

    int inputIdx = 0, outputIdx = 0;
    for (const auto &val : nodesArr.GetArray()) {
        if (!val.IsObject()) continue;
        const auto &obj = val.GetObject();
        QString type = QString::fromUtf8(obj["type"].GetString(), (int)obj["type"].GetStringLength());

        if (type == "CanvasInput") {
            QString name;
            if (obj.HasMember("data") && obj["data"].IsObject() && obj["data"].HasMember("name"))
                name = QString::fromUtf8(obj["data"]["name"].GetString(), (int)obj["data"]["name"].GetStringLength());
            if (name.isEmpty()) name = QStringLiteral("input_%1").arg(inputIdx);
            int pt = static_cast<int>(PortType::Generic);
            if (obj.HasMember("data") && obj["data"].IsObject() && obj["data"].HasMember("portType"))
                pt = obj["data"]["portType"].GetInt();
            qmlAddInputPort(nodeId, name, pt);
            inputIdx++;
        } else if (type == "CanvasOutput") {
            QString name;
            if (obj.HasMember("data") && obj["data"].IsObject() && obj["data"].HasMember("name"))
                name = QString::fromUtf8(obj["data"]["name"].GetString(), (int)obj["data"]["name"].GetStringLength());
            if (name.isEmpty()) name = QStringLiteral("output_%1").arg(outputIdx);
            int pt = static_cast<int>(PortType::Generic);
            if (obj.HasMember("data") && obj["data"].IsObject() && obj["data"].HasMember("portType"))
                pt = obj["data"]["portType"].GetInt();
            qmlAddOutputPort(nodeId, name, pt);
            outputIdx++;
        }
    }
}

// ══════════════════════════════════════════════════════════════
// Static helpers
// ══════════════════════════════════════════════════════════════

int GraphModel::portTypeToInt(PortType t) { return static_cast<int>(t); }

QString GraphModel::portTypeColor(int portType)
{
    switch (static_cast<PortType>(portType)) {
    case PortType::Int:         return "#4A9EFF";
    case PortType::Float:       return "#4CDF8B";
    case PortType::String:      return "#FF9F43";
    case PortType::Bool:        return "#FF6B6B";
    case PortType::Color:       return "#A29BFE";
    case PortType::Generic:     return "#888888";
    case PortType::Double:      return "#F0DB4F";
    case PortType::Vec2:
    case PortType::Vec3:
    case PortType::Vec4:        return "#E17055";
    case PortType::Array:
    case PortType::Map:
    case PortType::JSON:        return "#00CEC9";
    case PortType::Image:       return "#FD79A8";
    case PortType::AudioBuffer: return "#6C5CE7";
    }
    return "#888888";
}

QString GraphModel::portTypeName(int portType)
{
    switch (static_cast<PortType>(portType)) {
    case PortType::Int:         return "Int";
    case PortType::Float:       return "Float";
    case PortType::String:      return "String";
    case PortType::Bool:        return "Bool";
    case PortType::Color:       return "Color";
    case PortType::Generic:     return "Generic";
    case PortType::Double:      return "Double";
    case PortType::Vec2:        return "Vec2";
    case PortType::Vec3:        return "Vec3";
    case PortType::Vec4:        return "Vec4";
    case PortType::Array:       return "Array";
    case PortType::Map:         return "Map";
    case PortType::JSON:        return "JSON";
    case PortType::Image:       return "Image";
    case PortType::AudioBuffer: return "Audio Buffer";
    }
    return "Unknown";
}

QString GraphModel::qmlScreenColorAt(int x, int y)
{
    QScreen *screen = QGuiApplication::screenAt(QPoint(x, y));
    if (!screen) screen = QGuiApplication::primaryScreen();
    if (!screen) return "#00000000";
    QPixmap screenshot = screen->grabWindow(0);
    QPoint topLeft = screen->geometry().topLeft();
    QColor color = screenshot.toImage().pixelColor(x - topLeft.x(), y - topLeft.y());
    return color.name(QColor::HexArgb);
}

// ══════════════════════════════════════════════════════════════
// Serialization (RapidJSON)
// ══════════════════════════════════════════════════════════════

QString GraphModel::qmlSerializeToJson() const
{
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

    writer.StartObject();
    writer.Key("nodes");
    writer.StartArray();

    for (const auto &n : m_graphNodes) {
        writer.StartObject();
        writer.Key("id");   writer.Uint64(n.id);
        writer.Key("type"); writer.String(n.type.toUtf8().constData(), (rapidjson::SizeType)n.type.toUtf8().size());
        const auto *ui = nodeUIState(n.id);
        writer.Key("x"); writer.Double(ui ? ui->x : 0.0);
        writer.Key("y"); writer.Double(ui ? ui->y : 0.0);

        // Serialize runtime data
        writer.Key("data");
        const auto *rt = nodeRuntime(n.id);
        if (rt) {
            writer.StartObject();
            for (auto it = rt->data.begin(); it != rt->data.end(); ++it) {
                writer.Key(it.key().toUtf8().constData(), (rapidjson::SizeType)it.key().toUtf8().size());
                // Convert QVariant to RapidJSON inline using a temp doc
                rapidjson::Document tmpDoc;
                tmpDoc.SetObject();
                auto val = qvariantToJsonVal(it.value(), tmpDoc.GetAllocator());
                val.Accept(writer);
            }
            writer.EndObject();
        } else {
            writer.StartObject(); writer.EndObject();
        }
        writer.EndObject();
    }

    writer.EndArray();
    writer.Key("edges");
    writer.StartArray();

    for (const auto &e : m_graphEdges) {
        writer.StartObject();
        writer.Key("id");            writer.Uint64(e.id);
        writer.Key("sourceNodeId"); writer.Uint64(e.sourceNodeId);
        writer.Key("sourcePort");   writer.String(e.sourcePort.toUtf8().constData(),
                                                    (rapidjson::SizeType)e.sourcePort.toUtf8().size());
        writer.Key("targetNodeId"); writer.Uint64(e.targetNodeId);
        writer.Key("targetPort");   writer.String(e.targetPort.toUtf8().constData(),
                                                    (rapidjson::SizeType)e.targetPort.toUtf8().size());
        writer.EndObject();
    }

    writer.EndArray();
    writer.EndObject();

    return QString::fromUtf8(buf.GetString(), (int)buf.GetSize());
}

void GraphModel::qmlDeserializeFromJson(const QString &json)
{
    clear();

    QByteArray raw = json.toUtf8();
    rapidjson::Document doc;
    doc.Parse(raw.constData());

    if (doc.HasParseError()) {
        qWarning() << "qmlDeserializeFromJson:" << rapidjson::GetParseError_En(doc.GetParseError());
        return;
    }
    if (!doc.IsObject()) return;

    // ── Nodes ──
    if (doc.HasMember("nodes") && doc["nodes"].IsArray()) {
        for (const auto &val : doc["nodes"].GetArray()) {
            if (!val.IsObject()) continue;
            const auto &obj = val.GetObject();

            NodeID id = obj["id"].GetUint64();
            QString type = QString::fromUtf8(obj["type"].GetString(), (int)obj["type"].GetStringLength());

            GraphNode gn;
            gn.id = id;
            gn.type = type;

            NodeRuntime rt;
            rt.evalVersion = 0;

            if (auto *typeInfo = nodeTypeInfo(type)) {
                for (auto it = typeInfo->inputs.begin(); it != typeInfo->inputs.end(); ++it)
                    gn.inputs[it.key()] = it.value();
                for (auto it = typeInfo->outputs.begin(); it != typeInfo->outputs.end(); ++it)
                    gn.outputs[it.key()] = it.value();
            }

            NodeUIState ui;
            ui.x = obj["x"].GetDouble();
            ui.y = obj["y"].GetDouble();

            // Restore data
            if (obj.HasMember("data") && obj["data"].IsObject()) {
                for (const auto &dataMember : obj["data"].GetObject()) {
                    QString key = QString::fromUtf8(dataMember.name.GetString(),
                                                    (int)dataMember.name.GetStringLength());
                    rt.data[key] = jsonValToQVariant(dataMember.value);
                }
            }

            m_graphNodes.insert(id, std::move(gn));
            m_runtimes.insert(id, std::move(rt));
            m_uiStates.insert(id, std::move(ui));

            emit nodeAdded(id);
            emit qmlNodeAdded(idToStr(id));
        }
    }

    // ── Edges ──
    if (doc.HasMember("edges") && doc["edges"].IsArray()) {
        for (const auto &val : doc["edges"].GetArray()) {
            if (!val.IsObject()) continue;
            const auto &obj = val.GetObject();

            GraphEdge e;
            e.id = obj["id"].GetUint64();
            e.sourceNodeId = obj["sourceNodeId"].GetUint64();
            e.sourcePort = QString::fromUtf8(obj["sourcePort"].GetString(), (int)obj["sourcePort"].GetStringLength());
            e.targetNodeId = obj["targetNodeId"].GetUint64();
            e.targetPort = QString::fromUtf8(obj["targetPort"].GetString(), (int)obj["targetPort"].GetStringLength());

            m_graphEdges.insert(e.id, std::move(e));
            emit edgeAdded(e.id);
            emit qmlEdgeAdded(idToStr(e.id));
        }
    }

    m_topology.invalidate();
}

void GraphModel::clear()
{
    auto edgeSnapshot = m_graphEdges;
    auto nodeSnapshot = m_graphNodes;
    m_graphEdges.clear();
    m_graphNodes.clear();
    m_runtimes.clear();
    m_uiStates.clear();
    m_topology.invalidate();
    for (const auto &e : edgeSnapshot) { emit edgeRemoved(e.id); emit qmlEdgeRemoved(idToStr(e.id)); }
    for (const auto &n : nodeSnapshot) { emit nodeRemoved(n.id); emit qmlNodeRemoved(idToStr(n.id)); }
}

void GraphModel::qmlCopyRegistryFrom(GraphModel *source)
{
    if (!source) return;
    m_nodeTypes = source->m_nodeTypes;
    m_categories = source->m_categories;
}

bool GraphModel::qmlSaveToFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(qmlSerializeToJson().toUtf8());
    file.close();
    return true;
}

bool GraphModel::qmlLoadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;
    QString json = QString::fromUtf8(file.readAll());
    file.close();
    qmlDeserializeFromJson(json);
    return true;
}

} // namespace NodeEditor
