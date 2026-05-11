#include <QtTest>
#include <QtTest/QtTest>
#include "NodeEditor/GraphModel.h"
#include "NodeEditor/DataFlowEngine.h"

using namespace NodeEditor;

class TestDataFlowEngine : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qRegisterMetaType<QUuid>("QUuid");
        qRegisterMetaType<QList<QUuid>>("QList<QUuid>");
    }

    void testPropagationOrder()
    {
        GraphModel model;
        DataFlowEngine engine(&model);

        QUuid a = model.addNode("A");
        QUuid b = model.addNode("B");
        QUuid c = model.addNode("C");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", c, "in");

        QList<QUuid> computed;
        QSignalSpy spy(&engine, &DataFlowEngine::propagationComplete);
        engine.processAll();

        QCOMPARE(spy.count(), 1);
        computed = spy[0][0].value<QList<QUuid>>();
        QCOMPARE(computed.size(), 3);
        QCOMPARE(computed[0], a);
        QCOMPARE(computed[1], b);
        QCOMPARE(computed[2], c);
    }

    void testCycleEmitsError()
    {
        GraphModel model;
        DataFlowEngine engine(&model);

        QUuid a = model.addNode("A");
        QUuid b = model.addNode("B");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", a, "in");

        QSignalSpy spy(&engine, &DataFlowEngine::cycleDetected);
        engine.processAll();
        QCOMPARE(spy.count(), 1);
    }

    void testNodeChangePropagatesDownstream()
    {
        GraphModel model;
        DataFlowEngine engine(&model);

        QUuid a = model.addNode("A");
        QUuid b = model.addNode("B");
        QUuid c = model.addNode("C");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", c, "in");

        QSignalSpy spy(&engine, &DataFlowEngine::propagationComplete);
        engine.processNodeChange(a);

        QCOMPARE(spy.count(), 1);
        auto computed = spy[0][0].value<QList<QUuid>>();
        QCOMPARE(computed.size(), 3);
    }
};

QTEST_MAIN(TestDataFlowEngine)
#include "tst_DataFlowEngine.moc"
