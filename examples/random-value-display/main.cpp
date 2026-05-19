#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/UndoManager.h"
#include "NodeEditor/BaseNode.h"
#include "NodeEditor/DefaultNodes.h"
#include "NodeEditor/PreviewManager.h"

using namespace NodeEditor;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<GraphModel>("NodeEditor", 1, 0, "GraphModel");
    qmlRegisterType<UndoManager>("NodeEditor", 1, 0, "UndoManager");
    qmlRegisterType<DataFlowEngine>("NodeEditor", 1, 0, "DataFlowEngine");

    GraphModel model;
    registerDefaultNodeTypes(&model);

    DataFlowEngine engine(&model);
    engine.setAutoCompute(false);

    PreviewManager previewManager(&model, &engine);

    // Graph: Constant → LED Matrix
    NodeID constId = model.addNode("utility/value/constant", QPointF(50, 50));
    NodeID ledId = model.addNode("output/display/ledMatrix", QPointF(280, 50));
    model.connectPorts(constId, QStringLiteral("output"),
                       ledId, QStringLiteral("value"));
    model.setNodeData(constId, QStringLiteral("value"), 42.0);
    model.setNodeData(ledId, QStringLiteral("width"), 32);
    model.setNodeData(ledId, QStringLiteral("height"), 8);

    Q_UNUSED(constId)

    qDebug() << "Graph setup complete, loading QML...";

    QQmlApplicationEngine qmlEngine;
    qmlEngine.rootContext()->setContextProperty("_nodeTypeRegistry", &model);
    qmlEngine.rootContext()->setContextProperty("_previewManager", &previewManager);
    qmlEngine.addImportPath(QCoreApplication::applicationDirPath() + "/../..");

    QObject::connect(
        &qmlEngine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    qmlEngine.loadFromModule("RandomValueDisplay", "Main");

    qDebug() << "QML loaded, entering event loop...";

    return QCoreApplication::exec();
}
