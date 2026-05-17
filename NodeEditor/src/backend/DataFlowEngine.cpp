#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/BaseNode.h"
#include <QSet>

namespace NodeEditor {

DataFlowEngine::DataFlowEngine(QObject *parent) : QObject(parent) {}

DataFlowEngine::DataFlowEngine(GraphModel *model, QObject *parent)
    : QObject(parent), m_model(model)
{
    wireConnections();
}

GraphModel *DataFlowEngine::graphModel() const { return m_model; }

void DataFlowEngine::setGraphModel(GraphModel *model)
{
    if (m_model == model) return;
    unwireConnections();
    m_model = model;
    wireConnections();
    emit graphModelChanged();
}

bool DataFlowEngine::autoCompute() const { return m_autoCompute; }

void DataFlowEngine::setAutoCompute(bool enabled)
{
    if (m_autoCompute == enabled) return;
    m_autoCompute = enabled;
    emit autoComputeChanged();
}

BaseNode *DataFlowEngine::getOrCreateNode(const QString &type)
{
    auto it = m_nodeInstances.find(type);
    if (it != m_nodeInstances.end())
        return it.value();
    auto *instance = BaseNode::create(type);
    if (instance) m_nodeInstances[type] = instance;
    return instance;
}

void DataFlowEngine::clearCache()
{
    qDeleteAll(m_nodeInstances);
    m_nodeInstances.clear();
}

// ── Dirty-DAG-aware execution ─────────────────────────────────

bool DataFlowEngine::executeNode(uint64_t nodeId, QList<uint64_t> &computedOrder)
{
    const auto *gn = m_model->graphNode(nodeId);
    if (!gn) return false;

    auto *instance = getOrCreateNode(gn->type);
    if (!instance) {
        computedOrder.append(nodeId);
        return true;
    }

    auto *rt = m_model->nodeRuntime(nodeId);
    if (!rt) return false;

    // Gather inputs from connected upstream nodes or stored data
    QVariantMap inputs;
    for (auto it = gn->inputs.begin(); it != gn->inputs.end(); ++it) {
        const QString &portName = it.key();
        bool connected = false;

        for (const auto &e : m_model->allGraphEdges()) {
            if (e.targetNodeId == nodeId && e.targetPort == portName) {
                QVariant srcVal = m_model->nodeData(e.sourceNodeId, e.sourcePort);
                if (srcVal.isValid()) {
                    inputs[portName] = srcVal;
                    connected = true;
                }
                break;
            }
        }

        if (!connected) {
            QVariant stored = m_model->nodeData(nodeId, portName);
            inputs[portName] = stored.isValid() ? stored : it.value().defaultValue;
        }
    }

    // ── Dirty DAG: skip if not in dirty set ──
    if (!m_dirtyNodes.contains(nodeId)) {
        computedOrder.append(nodeId);
        return true;
    }

    // Compute
    QVariantMap outputs = instance->compute(inputs);

    // Store outputs
    for (auto it = outputs.begin(); it != outputs.end(); ++it)
        m_model->setNodeData(nodeId, it.key(), it.value());

    // Write resolved inputs back for QML display
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        QVariant current = m_model->nodeData(nodeId, it.key());
        if (current != it.value())
            m_model->setNodeData(nodeId, it.key(), it.value());
    }

    // Mark downstream as dirty
    for (const auto &e : m_model->allGraphEdges())
        if (e.sourceNodeId == nodeId)
            m_dirtyNodes.insert(e.targetNodeId);

    rt->evalVersion++;
    instance->setDirty(false);
    computedOrder.append(nodeId);
    return true;
}

// ── processNodeChange with dirty propagation ──────────────────

void DataFlowEngine::processNodeChange(uint64_t nodeId)
{
    if (m_processing || !m_autoCompute || !m_model) return;
    m_processing = true;

    if (m_model->hasCycles()) {
        emit cycleDetected();
        m_processing = false;
        return;
    }

    // Mark changed node + all downstream as dirty
    m_dirtyNodes.clear();
    m_dirtyNodes.insert(nodeId);

    QList<uint64_t> queue = {nodeId};
    QSet<uint64_t> visited = {nodeId};
    while (!queue.isEmpty()) {
        uint64_t current = queue.takeFirst();
        for (const auto &e : m_model->allGraphEdges()) {
            if (e.sourceNodeId == current && !visited.contains(e.targetNodeId)) {
                m_dirtyNodes.insert(e.targetNodeId);
                visited.insert(e.targetNodeId);
                queue.append(e.targetNodeId);
            }
        }
    }

    // Execute only dirty nodes in topological order
    const auto &sorted = m_model->cachedTopologicalOrder();
    QList<uint64_t> computed;

    for (uint64_t id : sorted) {
        if (m_dirtyNodes.contains(id) || id == nodeId) {
            if (!executeNode(id, computed))
                break;
        }
    }

    m_dirtyNodes.clear();
    m_processing = false;
    emit propagationComplete(computed);
}

void DataFlowEngine::processAll()
{
    if (!m_model || m_model->hasCycles()) {
        emit cycleDetected();
        return;
    }

    const auto &sorted = m_model->cachedTopologicalOrder();
    m_processing = true;

    m_dirtyNodes.clear();
    for (uint64_t id : sorted)
        m_dirtyNodes.insert(id);

    QList<uint64_t> computed;
    for (uint64_t id : sorted) {
        if (!executeNode(id, computed))
            break;
    }

    m_dirtyNodes.clear();
    m_processing = false;
    emit propagationComplete(computed);
}

// ── Pull evaluation ──────────────────────────────────────────

QVariant DataFlowEngine::requestValue(uint64_t nodeId, const QString &port)
{
    if (!m_model) return {};

    // 1. Find all upstream nodes needed for this node's inputs (backward BFS)
    QSet<uint64_t> upstream;
    QList<uint64_t> queue = {nodeId};
    upstream.insert(nodeId);
    while (!queue.isEmpty()) {
        uint64_t cur = queue.takeFirst();
        const auto *gn = m_model->graphNode(cur);
        if (!gn) continue;

        // For each input port, find the edge that connects it
        for (auto it = gn->inputs.begin(); it != gn->inputs.end(); ++it) {
            for (const auto &e : m_model->allGraphEdges()) {
                if (e.targetNodeId == cur && e.targetPort == it.key()) {
                    if (!upstream.contains(e.sourceNodeId)) {
                        upstream.insert(e.sourceNodeId);
                        queue.append(e.sourceNodeId);
                    }
                    break;
                }
            }
        }
    }

    // 2. Compute dirty subset: only upstream nodes that need recomputing
    //    (if no m_dirtyNodes context, compute all upstream)
    QSet<uint64_t> toCompute;
    if (m_dirtyNodes.isEmpty()) {
        toCompute = upstream;
    } else {
        // Only compute intersection of dirty + upstream
        for (uint64_t id : upstream) {
            if (m_dirtyNodes.contains(id))
                toCompute.insert(id);
        }
        // Always compute the requested node itself
        toCompute.insert(nodeId);
    }

    if (toCompute.isEmpty()) {
        // Nothing dirty — return cached value
        return m_model->nodeData(nodeId, port);
    }

    // 3. Evaluate dirty upstream nodes in topological order
    const auto &sorted = m_model->cachedTopologicalOrder();
    QList<uint64_t> computed;

    for (uint64_t id : sorted) {
        if (toCompute.contains(id)) {
            if (!executeNode(id, computed))
                break;
        }
    }

    // 4. Return the requested port value
    return m_model->nodeData(nodeId, port);
}

QVariant DataFlowEngine::qmlRequestValue(const QString &nodeId, const QString &port)
{
    return requestValue(strToId(nodeId), port);
}

// ── Signal wiring ─────────────────────────────────────────────

void DataFlowEngine::wireConnections()
{
    if (!m_model) return;
    m_connections.append(connect(m_model, &GraphModel::nodeDataChanged, this,
        [this](uint64_t nodeId, const QString &) { processNodeChange(nodeId); }));
    m_connections.append(connect(m_model, &GraphModel::nodeAdded, this,
        [this](uint64_t nodeId) { processNodeChange(nodeId); }));
    m_connections.append(connect(m_model, &GraphModel::edgeAdded, this,
        [this](uint64_t edgeId) {
            const auto *e = m_model->graphEdge(edgeId);
            if (e) processNodeChange(e->targetNodeId);
        }));
}

void DataFlowEngine::unwireConnections()
{
    for (auto &c : m_connections)
        disconnect(c);
    m_connections.clear();
}

} // namespace NodeEditor
