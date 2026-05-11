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
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

// ── JsonInputNode ───────────────────────────────────────────

QList<PortInfo> JsonInputNode::inputSpec() const
{
    return {{PortType::String, "filePath", QVariant("")}};
}

QList<PortInfo> JsonInputNode::outputSpec() const
{
    return {{PortType::Generic, "output", QVariant()}};
}

QVariantMap JsonInputNode::compute(const QVariantMap &inputs)
{
    QVariantMap out;
    QString filePath = inputs.value("filePath").toString().trimmed();
    if (filePath.isEmpty()) {
        out["output"] = QVariant();
        return out;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "JsonInputNode: cannot open file:" << filePath;
        out["output"] = QVariant();
        return out;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "JsonInputNode: JSON parse error:" << err.errorString();
        out["output"] = QString::fromUtf8(data);
        return out;
    }

    if (doc.isObject())
        out["output"] = doc.object().toVariantMap();
    else if (doc.isArray())
        out["output"] = doc.array().toVariantList();
    else
        out["output"] = QString::fromUtf8(data);

    return out;
}

QString JsonInputNode::nodeType() const { return "JsonInput"; }
QString JsonInputNode::nodeName() const { return "JSON Input"; }
QString JsonInputNode::nodeCategory() const { return "Input"; }
QString JsonInputNode::nodeSubCategory() const { return "File"; }
QString JsonInputNode::displayColor() const { return "#00CEC9"; }

// ── Registration ────────────────────────────────────────────

void registerDefaultNodeTypes(GraphModel *model)
{
    if (!model) return;

    // --- Default/legacy node registrations (CanvasInput, CanvasOutput, JsonInput) ---
    model->registerCategory({"Input", "Input", QColor("#4CDF8B")});
    model->registerCategory({"SubGraph", "SubGraph", QColor("#6C5CE7")});

    BaseNode::registerType("CanvasInput", []() { return new CanvasInputNode(); });
    BaseNode::registerType("CanvasOutput", []() { return new CanvasOutputNode(); });
    BaseNode::registerType("JsonInput", []() { return new JsonInputNode(); });

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
    {
        NodeTypeInfo info;
        info.inputs["filePath"] = PortInfo{PortType::String, "filePath", QVariant("")};
        info.outputs["output"] = PortInfo{PortType::Generic, "output", QVariant()};
        info.displayColor = "#00CEC9";
        info.categoryId = "Input";
        info.subCategory = "File";
        info.nodeName = "JSON Input";
        model->registerNodeType("JsonInput", info);
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