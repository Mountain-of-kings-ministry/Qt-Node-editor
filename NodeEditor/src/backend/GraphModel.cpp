#include "NodeEditor/GraphModel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSet>
#include <algorithm>

namespace NodeEditor {

GraphModel::GraphModel(QObject *parent)
    : QObject(parent)
{
}

// ── Internal C++ API ──────────────────────────────────────

QUuid GraphModel::addNode(const QString &type, const QPointF &position, const QUuid &existingId)
{
    QUuid id = existingId.isNull() ? QUuid::createUuid() : existingId;
    NodeData data;
    data.id = id;
    data.type = type;
    data.position = position;

    if (auto *info = nodeTypeInfo(type)) {
        for (auto it = info->inputs.begin(); it != info->inputs.end(); ++it) {
            data.inputs[it.key()] = it.value();
            if (it.value().defaultValue.isValid())
                data.data[it.key()] = it.value().defaultValue;
        }
        for (auto it = info->outputs.begin(); it != info->outputs.end(); ++it) {
            data.outputs[it.key()] = it.value();
            if (it.value().defaultValue.isValid())
                data.data[it.key()] = it.value().defaultValue;
        }
    }

    m_nodes.append(data);
    emit nodeAdded(id);
    emit qmlNodeAdded(uuidToStr(id));
    return id;
}

void GraphModel::removeNode(const QUuid &nodeId)
{
    QString idStr = uuidToStr(nodeId);

    // Collect downstream targets and removed edges before removal
    QList<QUuid> downstreamNodes;
    QList<QString> downstreamPorts;
    QList<QUuid> removedEdgeIds;
    QList<QString> removedEdgeIdStrs;
    for (const auto &e : m_edges) {
        if (e.sourceNodeId == nodeId) {
            downstreamNodes.append(e.targetNodeId);
            downstreamPorts.append(e.targetPort);
        }
        if (e.sourceNodeId == nodeId || e.targetNodeId == nodeId) {
            removedEdgeIds.append(e.id);
            removedEdgeIdStrs.append(uuidToStr(e.id));
        }
    }

    m_edges.erase(
        std::remove_if(m_edges.begin(), m_edges.end(),
            [&](const EdgeData &e) {
                return e.sourceNodeId == nodeId || e.targetNodeId == nodeId;
            }),
        m_edges.end());

    m_nodes.erase(
        std::remove_if(m_nodes.begin(), m_nodes.end(),
            [&](const NodeData &n) { return n.id == nodeId; }),
        m_nodes.end());

    // Notify QML about removed edges so they disappear from canvas
    for (int i = 0; i < removedEdgeIds.size(); ++i) {
        emit edgeRemoved(removedEdgeIds[i]);
        emit qmlEdgeRemoved(removedEdgeIdStrs[i]);
    }

    // Reset downstream input ports to their type defaults
    for (int i = 0; i < downstreamNodes.size(); ++i) {
        auto *targetNode = node(downstreamNodes[i]);
        if (!targetNode) continue;
        auto *typeInfo = nodeTypeInfo(targetNode->type);
        if (typeInfo && typeInfo->inputs.contains(downstreamPorts[i]))
            setNodeData(downstreamNodes[i], downstreamPorts[i],
                        typeInfo->inputs[downstreamPorts[i]].defaultValue);
    }

    emit nodeRemoved(nodeId);
    emit qmlNodeRemoved(idStr);
}

NodeData *GraphModel::node(const QUuid &nodeId)
{
    for (auto &n : m_nodes)
        if (n.id == nodeId)
            return &n;
    return nullptr;
}

const QList<NodeData> &GraphModel::nodes() const
{
    return m_nodes;
}

QUuid GraphModel::connectPorts(const QUuid &sourceNode, const QString &sourcePort,
                               const QUuid &targetNode, const QString &targetPort)
{
    EdgeData edge;
    edge.id = QUuid::createUuid();
    edge.sourceNodeId = sourceNode;
    edge.sourcePort = sourcePort;
    edge.targetNodeId = targetNode;
    edge.targetPort = targetPort;
    m_edges.append(edge);
    emit edgeAdded(edge.id);
    emit qmlEdgeAdded(uuidToStr(edge.id));
    return edge.id;
}

void GraphModel::disconnectEdge(const QUuid &edgeId)
{
    // Find edge info before removal
    QUuid targetNodeId;
    QString targetPort;
    for (const auto &e : m_edges) {
        if (e.id == edgeId) {
            targetNodeId = e.targetNodeId;
            targetPort = e.targetPort;
            break;
        }
    }

    QString idStr = uuidToStr(edgeId);
    m_edges.erase(
        std::remove_if(m_edges.begin(), m_edges.end(),
            [&](const EdgeData &e) { return e.id == edgeId; }),
        m_edges.end());

    // Reset target input to its type default
    if (!targetNodeId.isNull()) {
        auto *targetNode = node(targetNodeId);
        if (targetNode) {
            auto *typeInfo = nodeTypeInfo(targetNode->type);
            if (typeInfo && typeInfo->inputs.contains(targetPort))
                setNodeData(targetNodeId, targetPort,
                            typeInfo->inputs[targetPort].defaultValue);
        }
    }

    emit edgeRemoved(edgeId);
    emit qmlEdgeRemoved(idStr);
}

const QList<EdgeData> &GraphModel::edges() const
{
    return m_edges;
}

void GraphModel::setNodeData(const QUuid &nodeId, const QString &key, const QVariant &value)
{
    auto *n = node(nodeId);
    if (!n)
        return;
    n->data[key] = value;
    emit nodeDataChanged(nodeId, key);
    emit qmlNodeDataChanged(uuidToStr(nodeId), key);
}

QVariant GraphModel::nodeData(const QUuid &nodeId, const QString &key) const
{
    for (const auto &n : m_nodes)
        if (n.id == nodeId)
            return n.data.value(key);
    return {};
}

void GraphModel::setNodePosition(const QUuid &nodeId, const QPointF &position)
{
    auto *n = node(nodeId);
    if (!n)
        return;
    n->position = position;
    emit nodePositionChanged(nodeId);
    emit qmlNodePositionChanged(uuidToStr(nodeId));
}

QPointF GraphModel::nodePosition(const QUuid &nodeId) const
{
    for (const auto &n : m_nodes)
        if (n.id == nodeId)
            return n.position;
    return {};
}

QList<QUuid> GraphModel::topologicalSort() const
{
    QMap<QUuid, int> inDegree;
    QMap<QUuid, QList<QUuid>> adjacency;

    for (const auto &n : m_nodes) {
        inDegree[n.id] = 0;
        adjacency[n.id] = {};
    }

    for (const auto &e : m_edges) {
        adjacency[e.sourceNodeId].append(e.targetNodeId);
        inDegree[e.targetNodeId]++;
    }

    QList<QUuid> result;
    QList<QUuid> queue;

    for (auto it = inDegree.begin(); it != inDegree.end(); ++it)
        if (it.value() == 0)
            queue.append(it.key());

    while (!queue.isEmpty()) {
        QUuid id = queue.takeFirst();
        result.append(id);
        for (const auto &neighbor : adjacency[id]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0)
                queue.append(neighbor);
        }
    }

    return result;
}

bool GraphModel::hasCycles() const
{
    return topologicalSort().size() != m_nodes.size();
}

// ── Node type registry ────────────────────────────────────

void GraphModel::registerNodeType(const QString &type, const NodeTypeInfo &info)
{
    m_nodeTypes[type] = info;
}

const NodeTypeInfo *GraphModel::nodeTypeInfo(const QString &type) const
{
    auto it = m_nodeTypes.find(type);
    if (it != m_nodeTypes.end())
        return &it.value();
    return nullptr;
}

QStringList GraphModel::qmlAllNodeTypes() const
{
    QStringList result;
    QSet<QString> seen;
    for (const auto &cat : m_categories) {
        for (auto it = m_nodeTypes.begin(); it != m_nodeTypes.end(); ++it) {
            if (it.value().categoryId == cat.id) {
                result.append(it.key());
                seen.insert(it.key());
            }
        }
    }
    for (auto it = m_nodeTypes.begin(); it != m_nodeTypes.end(); ++it) {
        if (!seen.contains(it.key()))
            result.append(it.key());
    }
    return result;
}

// ── Category registry ────────────────────────────────────

void GraphModel::registerCategory(const NodeCategory &cat)
{
    for (auto &c : m_categories) {
        if (c.id == cat.id) {
            c = cat;
            return;
        }
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
    for (auto it = m_nodeTypes.begin(); it != m_nodeTypes.end(); ++it) {
        if (it.value().categoryId == categoryId)
            result.append(it.key());
    }
    return result;
}

// ── QML-friendly API ──────────────────────────────────────

QString GraphModel::qmlAddNode(const QString &type, double x, double y)
{
    return uuidToStr(addNode(type, QPointF(x, y)));
}

void GraphModel::qmlRemoveNode(const QString &nodeId)
{
    removeNode(strToUuid(nodeId));
}

QStringList GraphModel::qmlNodeIds() const
{
    QStringList ids;
    for (const auto &n : m_nodes)
        ids.append(uuidToStr(n.id));
    return ids;
}

QVariantMap GraphModel::qmlNodeInfo(const QString &nodeId) const
{
    QUuid id = strToUuid(nodeId);
    for (const auto &n : m_nodes) {
        if (n.id == id) {
            QVariantMap info;
            info["id"] = uuidToStr(n.id);
            info["type"] = n.type;
            info["x"] = n.position.x();
            info["y"] = n.position.y();

            QStringList inPorts, outPorts;
            QVariantList inPortTypes, outPortTypes;
            for (auto it = n.inputs.begin(); it != n.inputs.end(); ++it) {
                inPorts.append(it.key());
                inPortTypes.append(static_cast<int>(it.value().type));
            }
            for (auto it = n.outputs.begin(); it != n.outputs.end(); ++it) {
                outPorts.append(it.key());
                outPortTypes.append(static_cast<int>(it.value().type));
            }
            info["inputPorts"] = inPorts;
            info["inputPortTypes"] = inPortTypes;
            info["outputPorts"] = outPorts;
            info["outputPortTypes"] = outPortTypes;

            auto *typeInfo = nodeTypeInfo(n.type);
            info["color"] = typeInfo ? typeInfo->displayColor : "#4A9EFF";
            info["categoryId"] = typeInfo ? typeInfo->categoryId : QString();
            info["subCategory"] = typeInfo ? typeInfo->subCategory : QString();
            info["nodeName"] = typeInfo ? typeInfo->nodeName : QString();

            return info;
        }
    }
    return {};
}

void GraphModel::qmlSetNodePosition(const QString &nodeId, double x, double y)
{
    setNodePosition(strToUuid(nodeId), QPointF(x, y));
}

void GraphModel::qmlSetNodeData(const QString &nodeId, const QString &key, const QVariant &value)
{
    setNodeData(strToUuid(nodeId), key, value);
}

QVariant GraphModel::qmlNodeData(const QString &nodeId, const QString &key) const
{
    return nodeData(strToUuid(nodeId), key);
}

QString GraphModel::qmlConnectPorts(const QString &sourceNode, const QString &sourcePort,
                                    const QString &targetNode, const QString &targetPort)
{
    return uuidToStr(connectPorts(strToUuid(sourceNode), sourcePort,
                                  strToUuid(targetNode), targetPort));
}

void GraphModel::qmlDisconnectEdge(const QString &edgeId)
{
    disconnectEdge(strToUuid(edgeId));
}

QStringList GraphModel::qmlEdgeIds() const
{
    QStringList ids;
    for (const auto &e : m_edges)
        ids.append(uuidToStr(e.id));
    return ids;
}

QVariantMap GraphModel::qmlEdgeInfo(const QString &edgeId) const
{
    QUuid id = strToUuid(edgeId);
    for (const auto &e : m_edges) {
        if (e.id == id) {
            QVariantMap info;
            info["id"] = uuidToStr(e.id);
            info["sourceNodeId"] = uuidToStr(e.sourceNodeId);
            info["sourcePort"] = e.sourcePort;
            info["targetNodeId"] = uuidToStr(e.targetNodeId);
            info["targetPort"] = e.targetPort;
            return info;
        }
    }
    return {};
}

QStringList GraphModel::qmlNodeInputPorts(const QString &nodeId) const
{
    QUuid id = strToUuid(nodeId);
    for (const auto &n : m_nodes)
        if (n.id == id) {
            QStringList ports;
            for (auto it = n.inputs.begin(); it != n.inputs.end(); ++it)
                ports.append(it.key());
            return ports;
        }
    return {};
}

QStringList GraphModel::qmlNodeOutputPorts(const QString &nodeId) const
{
    QUuid id = strToUuid(nodeId);
    for (const auto &n : m_nodes)
        if (n.id == id) {
            QStringList ports;
            for (auto it = n.outputs.begin(); it != n.outputs.end(); ++it)
                ports.append(it.key());
            return ports;
        }
    return {};
}

int GraphModel::qmlPortType(const QString &nodeId, const QString &port, bool isInput) const
{
    QUuid id = strToUuid(nodeId);
    for (const auto &n : m_nodes) {
        if (n.id == id) {
            if (isInput) {
                auto it = n.inputs.find(port);
                if (it != n.inputs.end())
                    return static_cast<int>(it.value().type);
            } else {
                auto it = n.outputs.find(port);
                if (it != n.outputs.end())
                    return static_cast<int>(it.value().type);
            }
        }
    }
    return static_cast<int>(PortType::Generic);
}

bool GraphModel::qmlIsPortConnected(const QString &nodeId, const QString &port, bool isInput) const
{
    QUuid id = strToUuid(nodeId);
    for (const auto &e : m_edges) {
        if (isInput) {
            if (e.targetNodeId == id && e.targetPort == port)
                return true;
        } else {
            if (e.sourceNodeId == id && e.sourcePort == port)
                return true;
        }
    }
    return false;
}

// ── Dynamic port management ─────────────────────────────────

void GraphModel::qmlAddInputPort(const QString &nodeId, const QString &portName, int portType)
{
    QUuid id = strToUuid(nodeId);
    for (auto &n : m_nodes) {
        if (n.id == id) {
            if (n.inputs.contains(portName)) return;
            n.inputs[portName] = PortInfo{static_cast<PortType>(portType), portName, QVariant()};
            emit qmlNodePortsChanged(nodeId);
            emit nodeDataChanged(id, portName);
            return;
        }
    }
}

void GraphModel::qmlRemoveInputPort(const QString &nodeId, const QString &portName)
{
    QUuid id = strToUuid(nodeId);
    for (auto &n : m_nodes) {
        if (n.id == id) {
            n.inputs.remove(portName);
            n.data.remove(portName);
            emit qmlNodePortsChanged(nodeId);
            return;
        }
    }
}

void GraphModel::qmlAddOutputPort(const QString &nodeId, const QString &portName, int portType)
{
    QUuid id = strToUuid(nodeId);
    for (auto &n : m_nodes) {
        if (n.id == id) {
            if (n.outputs.contains(portName)) return;
            n.outputs[portName] = PortInfo{static_cast<PortType>(portType), portName, QVariant()};
            emit qmlNodePortsChanged(nodeId);
            emit nodeDataChanged(id, portName);
            return;
        }
    }
}

void GraphModel::qmlRemoveOutputPort(const QString &nodeId, const QString &portName)
{
    QUuid id = strToUuid(nodeId);
    for (auto &n : m_nodes) {
        if (n.id == id) {
            n.outputs.remove(portName);
            n.data.remove(portName);
            emit qmlNodePortsChanged(nodeId);
            return;
        }
    }
}

void GraphModel::qmlLoadCanvasFile(const QString &nodeId, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "qmlLoadCanvasFile: cannot open file:" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "qmlLoadCanvasFile: JSON parse error:" << err.errorString();
        return;
    }

    if (!doc.isObject()) return;

    QJsonObject rootObj = doc.object();
    QJsonArray nodesArr = rootObj["nodes"].toArray();

    int inputIdx = 0, outputIdx = 0;

    for (const auto &val : nodesArr) {
        QJsonObject obj = val.toObject();
        QString type = obj["type"].toString();
        QJsonObject dataObj = obj["data"].toObject();

        if (type == "CanvasInput") {
            QString name = dataObj["name"].toString();
            if (name.isEmpty()) {
                name = QStringLiteral("input_%1").arg(inputIdx);
            }
            int portType = static_cast<int>(PortType::Generic);
            if (dataObj.contains("portType"))
                portType = dataObj["portType"].toInt();
            qmlAddInputPort(nodeId, name, portType);
            inputIdx++;
        } else if (type == "CanvasOutput") {
            QString name = dataObj["name"].toString();
            if (name.isEmpty()) {
                name = QStringLiteral("output_%1").arg(outputIdx);
            }
            int portType = static_cast<int>(PortType::Generic);
            if (dataObj.contains("portType"))
                portType = dataObj["portType"].toInt();
            qmlAddOutputPort(nodeId, name, portType);
            outputIdx++;
        }
    }
}

// ── Static helpers ────────────────────────────────────────

int GraphModel::portTypeToInt(PortType t)
{
    return static_cast<int>(t);
}

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
    case PortType::Vec2:        return "#E17055";
    case PortType::Vec3:        return "#E17055";
    case PortType::Vec4:        return "#E17055";
    case PortType::Array:       return "#00CEC9";
    case PortType::Map:         return "#00CEC9";
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

// ── Serialization ───────────────────────────────────────────

QString GraphModel::qmlSerializeToJson() const
{
    QJsonObject rootObj;
    QJsonArray nodesArr;
    for (const auto &n : m_nodes) {
        QJsonObject nodeObj;
        nodeObj["id"] = uuidToStr(n.id);
        nodeObj["type"] = n.type;
        nodeObj["x"] = n.position.x();
        nodeObj["y"] = n.position.y();
        QJsonObject dataObj;
        for (auto it = n.data.begin(); it != n.data.end(); ++it)
            dataObj[it.key()] = QJsonValue::fromVariant(it.value());
        nodeObj["data"] = dataObj;
        nodesArr.append(nodeObj);
    }
    rootObj["nodes"] = nodesArr;

    QJsonArray edgesArr;
    for (const auto &e : m_edges) {
        QJsonObject edgeObj;
        edgeObj["id"] = uuidToStr(e.id);
        edgeObj["sourceNodeId"] = uuidToStr(e.sourceNodeId);
        edgeObj["sourcePort"] = e.sourcePort;
        edgeObj["targetNodeId"] = uuidToStr(e.targetNodeId);
        edgeObj["targetPort"] = e.targetPort;
        edgesArr.append(edgeObj);
    }
    rootObj["edges"] = edgesArr;

    QJsonDocument doc(rootObj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

void GraphModel::qmlDeserializeFromJson(const QString &json)
{
    clear();
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject rootObj = doc.object();

    QJsonArray nodesArr = rootObj["nodes"].toArray();
    for (const auto &val : nodesArr) {
        QJsonObject obj = val.toObject();
        QUuid id = strToUuid(obj["id"].toString());
        if (id.isNull()) continue;
        NodeData n;
        n.id = id;
        n.type = obj["type"].toString();
        n.position = QPointF(obj["x"].toDouble(), obj["y"].toDouble());
        QJsonObject dataObj = obj["data"].toObject();
        for (auto it = dataObj.begin(); it != dataObj.end(); ++it)
            n.data[it.key()] = it.value().toVariant();

        if (auto *info = nodeTypeInfo(n.type)) {
            for (auto it = info->inputs.begin(); it != info->inputs.end(); ++it)
                n.inputs[it.key()] = it.value();
            for (auto it = info->outputs.begin(); it != info->outputs.end(); ++it)
                n.outputs[it.key()] = it.value();
        }

        m_nodes.append(n);
        emit nodeAdded(n.id);
        emit qmlNodeAdded(uuidToStr(n.id));
    }

    QJsonArray edgesArr = rootObj["edges"].toArray();
    for (const auto &val : edgesArr) {
        QJsonObject obj = val.toObject();
        EdgeData e;
        e.id = strToUuid(obj["id"].toString());
        e.sourceNodeId = strToUuid(obj["sourceNodeId"].toString());
        e.sourcePort = obj["sourcePort"].toString();
        e.targetNodeId = strToUuid(obj["targetNodeId"].toString());
        e.targetPort = obj["targetPort"].toString();
        m_edges.append(e);
        emit edgeAdded(e.id);
        emit qmlEdgeAdded(uuidToStr(e.id));
    }
}

void GraphModel::clear()
{
    auto edgeIds = m_edges;
    auto nodeIds = m_nodes;
    m_edges.clear();
    m_nodes.clear();
    for (const auto &e : edgeIds) {
        emit edgeRemoved(e.id);
        emit qmlEdgeRemoved(uuidToStr(e.id));
    }
    for (const auto &n : nodeIds) {
        emit nodeRemoved(n.id);
        emit qmlNodeRemoved(uuidToStr(n.id));
    }
}

void GraphModel::qmlCopyRegistryFrom(GraphModel *source)
{
    if (!source) return;
    m_nodeTypes = source->m_nodeTypes;
    m_categories = source->m_categories;
}

// ── ID conversion ─────────────────────────────────────────

QString GraphModel::uuidToStr(const QUuid &id)
{
    return id.toString(QUuid::WithoutBraces);
}

QUuid GraphModel::strToUuid(const QString &str)
{
    return QUuid::fromString(str);
}

bool GraphModel::qmlSaveToFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.write(qmlSerializeToJson().toUtf8());
    file.close();
    return true;
}

bool GraphModel::qmlLoadFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QString json = QString::fromUtf8(file.readAll());
    file.close();
    qmlDeserializeFromJson(json);
    return true;
}

} // namespace NodeEditor
