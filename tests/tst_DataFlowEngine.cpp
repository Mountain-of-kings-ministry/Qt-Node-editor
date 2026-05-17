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
        qRegisterMetaType<uint64_t>("uint64_t");
        qRegisterMetaType<QList<uint64_t>>("QList<uint64_t>");
    }

    void testPropagationOrder()
    {
        GraphModel model;
        DataFlowEngine engine(&model);

        NodeID a = model.addNode("A");
        NodeID b = model.addNode("B");
        NodeID c = model.addNode("C");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", c, "in");

        QSignalSpy spy(&engine, &DataFlowEngine::propagationComplete);
        engine.processAll();

        QCOMPARE(spy.count(), 1);
        QList<uint64_t> computed = spy[0][0].value<QList<uint64_t>>();
        QCOMPARE(computed.size(), 3);
        QCOMPARE(computed[0], a);
        QCOMPARE(computed[1], b);
        QCOMPARE(computed[2], c);
    }

    void testCycleEmitsError()
    {
        GraphModel model;
        DataFlowEngine engine(&model);

        NodeID a = model.addNode("A");
        NodeID b = model.addNode("B");
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

        NodeID a = model.addNode("A");
        NodeID b = model.addNode("B");
        NodeID c = model.addNode("C");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", c, "in");

        QSignalSpy spy(&engine, &DataFlowEngine::propagationComplete);
        engine.processNodeChange(a);

        QCOMPARE(spy.count(), 1);
        QList<uint64_t> computed = spy[0][0].value<QList<uint64_t>>();
        QCOMPARE(computed.size(), 3);
    }
};

QTEST_MAIN(TestDataFlowEngine)
#include "tst_DataFlowEngine.moc"
