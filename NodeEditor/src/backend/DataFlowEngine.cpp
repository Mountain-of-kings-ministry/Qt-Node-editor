#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

DataFlowEngine::DataFlowEngine(GraphModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
{
}

BaseNode *DataFlowEngine::getOrCreateNode(const QString &type)
{
    auto it = m_nodeInstances.find(type);
    if (it != m_nodeInstances.end())
        return it.value();

    auto *instance = BaseNode::create(type);
    if (instance)
        m_nodeInstances[type] = instance;
    return instance;
}

void DataFlowEngine::clearCache()
{
    qDeleteAll(m_nodeInstances);
    m_nodeInstances.clear();
}

void DataFlowEngine::executeNode(const QUuid &nodeId)
{
    auto *data = m_model->node(nodeId);
    if (!data) return;

    auto *instance = getOrCreateNode(data->type);
    if (!instance) return;

    // Gather inputs: first from connected upstream nodes, then from stored data
    QVariantMap inputs;
    for (auto it = data->inputs.begin(); it != data->inputs.end(); ++it) {
        const QString &portName = it.key();

        // Check if this port is connected via an edge
        bool connected = false;
        for (const auto &e : m_model->edges()) {
            if (e.targetNodeId == nodeId && e.targetPort == portName) {
                // Read from source node's data
                auto *src = m_model->node(e.sourceNodeId);
                if (src) {
                    inputs[portName] = src->data.value(e.sourcePort);
                    connected = true;
                }
                break;
            }
        }

        // If not connected, use the node's stored data
        if (!connected)
            inputs[portName] = data->data.value(portName, it.value().defaultValue);
    }

    // Compute
    QVariantMap outputs = instance->compute(inputs);

    // Store output values back into the model
    for (auto it = outputs.begin(); it != outputs.end(); ++it)
        m_model->setNodeData(nodeId, it.key(), it.value());

    // Write resolved input values back so QML can display them
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        QVariant current = data->data.value(it.key());
        if (current != it.value())
            m_model->setNodeData(nodeId, it.key(), it.value());
    }

    instance->setDirty(false);
}

void DataFlowEngine::processNodeChange(const QUuid &nodeId)
{
    if (m_processing) return;
    m_processing = true;

    if (m_model->hasCycles()) {
        emit cycleDetected();
        m_processing = false;
        return;
    }

    auto sorted = m_model->topologicalSort();

    int startIndex = sorted.indexOf(nodeId);
    if (startIndex < 0) { m_processing = false; return; }

    QList<QUuid> computed;
    for (int i = startIndex; i < sorted.size(); ++i) {
        executeNode(sorted[i]);
        computed.append(sorted[i]);
    }

    m_processing = false;
    emit propagationComplete(computed);
}

void DataFlowEngine::processAll()
{
    if (m_model->hasCycles()) {
        emit cycleDetected();
        return;
    }

    auto sorted = m_model->topologicalSort();
    m_processing = true;

    QList<QUuid> computed;
    for (const auto &id : sorted) {
        executeNode(id);
        computed.append(id);
    }

    m_processing = false;
    emit propagationComplete(computed);
}

} // namespace NodeEditor
