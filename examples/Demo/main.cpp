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

    // Register ALL built-in nodes (Math, Color, Logic, Data, Events, Generators, Output, Qt, Utility, System, SubGraph)
    GraphModel model;
    registerDefaultNodeTypes(&model);

    // Register Demo-specific category and nodes
    model.registerCategory({"Demo", "Demo", QColor("#636E72")});

    BaseNode::registerType("demo/input", []() { return new DemoInputNode(); });
    BaseNode::registerType("demo/add", []() { return new DemoAddNode(); });
    BaseNode::registerType("demo/multiply", []() { return new DemoMultiplyNode(); });
    BaseNode::registerType("demo/output", []() { return new DemoOutputNode(); });

    {
        NodeTypeInfo info;
        info.inputs["input"] = PortInfo{PortType::Float, "input", QVariant(0.0)};
        info.outputs["value"] = PortInfo{PortType::Float, "value", QVariant()};
        info.displayColor = "#4CDF8B";
        info.categoryId = "Demo";
        info.subCategory = "Values";
        info.nodeName = "Demo Input";
        model.registerNodeType("demo/input", info);
    }
    {
        NodeTypeInfo info;
        info.inputs["a"] = PortInfo{PortType::Float, "a", QVariant(0.0)};
        info.inputs["b"] = PortInfo{PortType::Float, "b", QVariant(0.0)};
        info.outputs["result"] = PortInfo{PortType::Float, "result", QVariant()};
        info.displayColor = "#FF9F43";
        info.categoryId = "Demo";
        info.subCategory = "Math";
        info.nodeName = "Demo Add";
        model.registerNodeType("demo/add", info);
    }
    {
        NodeTypeInfo info;
        info.inputs["a"] = PortInfo{PortType::Float, "a", QVariant(1.0)};
        info.inputs["b"] = PortInfo{PortType::Float, "b", QVariant(1.0)};
        info.outputs["result"] = PortInfo{PortType::Float, "result", QVariant()};
        info.displayColor = "#4A9EFF";
        info.categoryId = "Demo";
        info.subCategory = "Math";
        info.nodeName = "Demo Multiply";
        model.registerNodeType("demo/multiply", info);
    }
    {
        NodeTypeInfo info;
        info.inputs["value"] = PortInfo{PortType::Float, "value", QVariant(0.0)};
        info.displayColor = "#FF6B6B";
        info.categoryId = "Demo";
        info.subCategory = "Debug";
        info.nodeName = "Demo Output";
        model.registerNodeType("demo/output", info);
    }

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
