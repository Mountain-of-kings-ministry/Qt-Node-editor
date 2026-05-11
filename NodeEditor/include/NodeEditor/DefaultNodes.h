#pragma once

#include "NodeEditor/GraphModel.h"
#include "NodeEditor/BaseNode.h"

namespace NodeEditor {

class GraphModel;

// CanvasInputNode: defines an input interface for a sub-graph
class CanvasInputNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;
    QVariantMap compute(const QVariantMap &inputs) override;
    QList<PortInfo> inputSpec() const override;
    QList<PortInfo> outputSpec() const override;
    QString nodeType() const override;
    QString nodeName() const override;
    QString nodeCategory() const override;
    QString nodeSubCategory() const override;
    QString displayColor() const override;
};

// CanvasOutputNode: defines an output interface for a sub-graph
class CanvasOutputNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;
    QVariantMap compute(const QVariantMap &inputs) override;
    QList<PortInfo> inputSpec() const override;
    QList<PortInfo> outputSpec() const override;
    QString nodeType() const override;
    QString nodeName() const override;
    QString nodeCategory() const override;
    QString nodeSubCategory() const override;
    QString displayColor() const override;
};

// JsonInputNode: loads a JSON file from disk and outputs parsed data
class JsonInputNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;
    QVariantMap compute(const QVariantMap &inputs) override;
    QList<PortInfo> inputSpec() const override;
    QList<PortInfo> outputSpec() const override;
    QString nodeType() const override;
    QString nodeName() const override;
    QString nodeCategory() const override;
    QString nodeSubCategory() const override;
    QString displayColor() const override;
};

// CanvasNode: loads a sub-graph JSON file and outputs parsed structure
class CanvasNode : public BaseNode {
    Q_OBJECT
public:
    using BaseNode::BaseNode;
    QVariantMap compute(const QVariantMap &inputs) override;
    QList<PortInfo> inputSpec() const override;
    QList<PortInfo> outputSpec() const override;
    QString nodeType() const override;
    QString nodeName() const override;
    QString nodeCategory() const override;
    QString nodeSubCategory() const override;
    QString displayColor() const override;
};

// Register all default node types, categories, and BaseNode factories on a model
void registerDefaultNodeTypes(GraphModel *model);

} // namespace NodeEditor