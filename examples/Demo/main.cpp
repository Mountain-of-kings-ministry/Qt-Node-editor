#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/UndoManager.h"
#include "NodeEditor/BaseNode.h"
#include "DemoNodes.h"

using namespace NodeEditor;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<GraphModel>("NodeEditor", 1, 0, "GraphModel", "Use context property");

    GraphModel model;
    DataFlowEngine engine(&model);

    // Register categories
    model.registerCategory({"Input", "Input", QColor("#4CDF8B")});
    model.registerCategory({"Math", "Math", QColor("#FF9F43")});
    model.registerCategory({"Output", "Output", QColor("#FF6B6B")});

    // Register demo node types
    BaseNode::registerType("Input", []() { return new InputNode(); });
    BaseNode::registerType("Add", []() { return new AddNode(); });
    BaseNode::registerType("Multiply", []() { return new MultiplyNode(); });
    BaseNode::registerType("Output", []() { return new OutputNode(); });

    // Register node type metadata for QML
    NodeTypeInfo inputInfo;
    inputInfo.outputs["value"] = PortInfo{PortType::Float, "value", QVariant()};
    inputInfo.displayColor = "#4CDF8B";
    inputInfo.categoryId = "Input";
    model.registerNodeType("Input", inputInfo);

    NodeTypeInfo addInfo;
    addInfo.inputs["a"] = PortInfo{PortType::Float, "a", QVariant(0.0)};
    addInfo.inputs["b"] = PortInfo{PortType::Float, "b", QVariant(0.0)};
    addInfo.outputs["result"] = PortInfo{PortType::Float, "result", QVariant()};
    addInfo.displayColor = "#FF9F43";
    addInfo.categoryId = "Math";
    addInfo.subCategory = "Basic Operations";
    model.registerNodeType("Add", addInfo);

    NodeTypeInfo multiplyInfo;
    multiplyInfo.inputs["a"] = PortInfo{PortType::Float, "a", QVariant(1.0)};
    multiplyInfo.inputs["b"] = PortInfo{PortType::Float, "b", QVariant(1.0)};
    multiplyInfo.outputs["result"] = PortInfo{PortType::Float, "result", QVariant()};
    multiplyInfo.displayColor = "#4A9EFF";
    multiplyInfo.categoryId = "Math";
    multiplyInfo.subCategory = "Basic Operations";
    model.registerNodeType("Multiply", multiplyInfo);

    NodeTypeInfo outputInfo;
    outputInfo.inputs["value"] = PortInfo{PortType::Float, "value", QVariant(0.0)};
    outputInfo.displayColor = "#FF6B6B";
    outputInfo.categoryId = "Output";
    outputInfo.subCategory = "Debug";
    model.registerNodeType("Output", outputInfo);

    // Auto-propagate when data changes
    QObject::connect(&model, &GraphModel::nodeDataChanged, [&](const QUuid &nodeId, const QString &) {
        engine.processNodeChange(nodeId);
    });

    UndoManager undoManager(&model);
    undoManager.clear();

    QQmlApplicationEngine engine_qqml;
    engine_qqml.rootContext()->setContextProperty("_graphModel", &model);
    engine_qqml.rootContext()->setContextProperty("_undoManager", &undoManager);
    engine_qqml.addImportPath(QCoreApplication::applicationDirPath() + "/../..");

    QObject::connect(
        &engine_qqml,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine_qqml.loadFromModule("NodeEditorDemo", "Main");

    return QCoreApplication::exec();
}
