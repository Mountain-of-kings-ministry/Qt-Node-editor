#include "NodeEditor/DefaultNodes.h"
#include "system/SystemNodes.h"
#include "math/MathNodes.h"
#include "color/ColorNodes.h"
#include "data/DataNodes.h"
#include "logic/LogicNodes.h"
#include "events/EventNodes.h"
#include "generators/GeneratorNodes.h"
#include "output/OutputNodes.h"
#include "qt/QtNodes.h"
#include "utility/UtilityNodes.h"


namespace NodeEditor {

// ── CanvasInputNode ─────────────────────────────────────────

QList<PortInfo> CanvasInputNode::inputSpec() const
{
    return {{PortType::Generic, "value", QVariant()}};
}

QList<PortInfo> CanvasInputNode::outputSpec() const
{
    return {{PortType::Generic, "value", QVariant()}};
}

QVariantMap CanvasInputNode::compute(const QVariantMap &inputs)
{
    QVariantMap out;
    out["value"] = inputs.value("value");
    return out;
}

QString CanvasInputNode::nodeType() const { return "CanvasInput"; }
QString CanvasInputNode::nodeName() const { return "Canvas Input"; }
QString CanvasInputNode::nodeCategory() const { return "SubGraph"; }
QString CanvasInputNode::nodeSubCategory() const { return "Interface"; }
QString CanvasInputNode::displayColor() const { return "#6C5CE7"; }

// ── CanvasOutputNode ────────────────────────────────────────

QList<PortInfo> CanvasOutputNode::inputSpec() const
{
    return {{PortType::Generic, "value", QVariant()}};
}

QList<PortInfo> CanvasOutputNode::outputSpec() const
{
    return {{PortType::Generic, "value", QVariant()}};
}

QVariantMap CanvasOutputNode::compute(const QVariantMap &inputs)
{
    QVariantMap out;
    out["value"] = inputs.value("value");
    return out;
}

QString CanvasOutputNode::nodeType() const { return "CanvasOutput"; }
QString CanvasOutputNode::nodeName() const { return "Canvas Output"; }
QString CanvasOutputNode::nodeCategory() const { return "SubGraph"; }
QString CanvasOutputNode::nodeSubCategory() const { return "Interface"; }
QString CanvasOutputNode::displayColor() const { return "#E17055"; }

// ── Registration ────────────────────────────────────────────

void registerDefaultNodeTypes(GraphModel *model)
{
    if (!model) return;

    // --- Default/legacy node registrations (CanvasInput, CanvasOutput) ---
    model->registerCategory({"SubGraph", "SubGraph", QColor("#6C5CE7")});

    BaseNode::registerType("CanvasInput", []() { return new CanvasInputNode(); });
    BaseNode::registerType("CanvasOutput", []() { return new CanvasOutputNode(); });

    {
        NodeTypeInfo info;
        info.inputs["value"] = PortInfo{PortType::Generic, "value", QVariant()};
        info.outputs["value"] = PortInfo{PortType::Generic, "value", QVariant()};
        info.displayColor = "#6C5CE7";
        info.categoryId = "SubGraph";
        info.subCategory = "Interface";
        info.nodeName = "Canvas Input";
        model->registerNodeType("CanvasInput", info);
    }
    {
        NodeTypeInfo info;
        info.inputs["value"] = PortInfo{PortType::Generic, "value", QVariant()};
        info.outputs["value"] = PortInfo{PortType::Generic, "value", QVariant()};
        info.displayColor = "#E17055";
        info.categoryId = "SubGraph";
        info.subCategory = "Interface";
        info.nodeName = "Canvas Output";
        model->registerNodeType("CanvasOutput", info);
    }

    // --- Category node registrations ---
    registerSystemNodeTypes(model);
    registerMathNodeTypes(model);
    registerColorNodeTypes(model);
    registerDataNodeTypes(model);
    registerLogicNodeTypes(model);
    registerEventNodeTypes(model);
    registerGeneratorNodeTypes(model);
    registerOutputNodeTypes(model);
    registerQtNodeTypes(model);
    registerUtilityNodeTypes(model);
}

} // namespace NodeEditor