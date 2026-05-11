#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/UndoManager.h"
#include "NodeEditor/BaseNode.h"
#include "NodeEditor/DefaultNodes.h"
#include "ScriptNodes.h"

using namespace NodeEditor;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<GraphModel>("NodeEditor", 1, 0, "GraphModel");
    qmlRegisterType<UndoManager>("NodeEditor", 1, 0, "UndoManager");
    qmlRegisterType<DataFlowEngine>("NodeEditor", 1, 0, "DataFlowEngine");

    GraphModel model;
    registerDefaultNodeTypes(&model);
    registerScriptNodeTypes(&model);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("_nodeTypeRegistry", &model);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/../..");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("ScriptDemo", "Main");

    return QCoreApplication::exec();
}
