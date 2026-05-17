#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QElapsedTimer>
#include <QDebug>
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/DataFlowEngine.h"
#include "NodeEditor/UndoManager.h"
#include "NodeEditor/BaseNode.h"
#include "NodeEditor/DefaultNodes.h"
#include "NodeEditor/PreviewManager.h"
#include "RandomValueNodes.h"

using namespace NodeEditor;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<GraphModel>("NodeEditor", 1, 0, "GraphModel");
    qmlRegisterType<UndoManager>("NodeEditor", 1, 0, "UndoManager");
    qmlRegisterType<DataFlowEngine>("NodeEditor", 1, 0, "DataFlowEngine");

    GraphModel model;
    registerDefaultNodeTypes(&model);
    registerRandomValueNodeTypes(&model);

    DataFlowEngine engine(&model);
    engine.setAutoCompute(true);

    PreviewManager previewManager(&model, &engine);
    previewManager.setRenderBudget(1);
    previewManager.setMaxCacheSize(100);

    // Build the test graph programmatically
    NodeID timerId = model.addNode("system/time/msTick", QPointF(50, 50));
    NodeID randomId = model.addNode("utility/random/range", QPointF(280, 50));
    NodeID ledId = model.addNode("output/display/ledMatrix", QPointF(510, 50));

    // Wire: timer.tick -> random.trigger
    model.connectPorts(timerId, QStringLiteral("tick"),
                       randomId, QStringLiteral("trigger"));

    // Wire: random.value -> ledMatrix.value
    model.connectPorts(randomId, QStringLiteral("value"),
                       ledId, QStringLiteral("value"));

    // Set LED matrix display size to 32x16 for better visual
    model.setNodeData(ledId, QStringLiteral("width"), 32);
    model.setNodeData(ledId, QStringLiteral("height"), 16);

    // Expose to QML
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

    // Battle test: drive timer at 1ms interval
    uint64_t tickCounter = 0;
    QElapsedTimer perfTimer;
    perfTimer.start();
    int frameCount = 0;

    QTimer driveTimer;
    QObject::connect(&driveTimer, &QTimer::timeout, [&]() {
        tickCounter++;
        model.setNodeData(timerId, QStringLiteral("trigger"),
                          static_cast<double>(tickCounter));

        // Request preview for the LED matrix node each tick
        previewManager.requestPreview(idToStr(ledId));

        frameCount++;
        if (perfTimer.elapsed() >= 1000) {
            qDebug() << "Random Value Display:"
                     << frameCount << "fps,"
                     << "cache size:" << previewManager.maxCacheSize();
            frameCount = 0;
            perfTimer.restart();
        }
    });
    driveTimer.setTimerType(Qt::PreciseTimer);
    driveTimer.start(1);

    return QCoreApplication::exec();
}
