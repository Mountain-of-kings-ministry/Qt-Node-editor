#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/UndoManager.h"
#include "NodeEditor/BaseNode.h"
#include "NodeEditor/DefaultNodes.h"
#include "DemoNodes.h"

using namespace NodeEditor;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<GraphModel>("NodeEditor", 1, 0, "GraphModel");
    qmlRegisterType<UndoManager>("NodeEditor", 1, 0, "UndoManager");
    qmlRegisterType<DataFlowEngine>("NodeEditor", 1, 0, "DataFlowEngine");

    // Register categories
    GraphModel model;
    model.registerCategory({"Input", "Input", QColor("#4CDF8B")});
    model.registerCategory({"Math", "Math", QColor("#FF9F43")});
    model.registerCategory({"Output", "Output", QColor("#FF6B6B")});

    // Register demo node types
    BaseNode::registerType("Input", []() { return new InputNode(); });
    BaseNode::registerType("Add", []() { return new AddNode(); });
    BaseNode::registerType("Multiply", []() { return new MultiplyNode(); });
    BaseNode::registerType("Output", []() { return new OutputNode(); });

    // Register system-default node types (CanvasInput, CanvasOutput, JsonInput, CanvasNode)
    registerDefaultNodeTypes(&model);

    // Register node type metadata for QML
    NodeTypeInfo inputInfo;
    inputInfo.inputs["input"] = PortInfo{PortType::Float, "input", QVariant(0.0)};
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

    // Pass registration model via context property so tabs can reference it
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("_nodeTypeRegistry", &model);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/../..");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("NodeEditorDemo", "Main");

    return QCoreApplication::exec();
}
