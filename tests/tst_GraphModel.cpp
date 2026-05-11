#include <QtTest>
#include <QtTest/QtTest>
#include "NodeEditor/GraphModel.h"

using namespace NodeEditor;

class TestGraphModel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qRegisterMetaType<QUuid>("QUuid");
    }

    void testAddNode()
    {
        GraphModel model;
        QUuid id = model.addNode("TestNode", QPointF(100, 200));
        QVERIFY(!id.isNull());
        QCOMPARE(model.nodes().size(), 1);
        QCOMPARE(model.nodes()[0].type, "TestNode");
        QCOMPARE(model.nodes()[0].position, QPointF(100, 200));
    }

    void testRemoveNode()
    {
        GraphModel model;
        QUuid id = model.addNode("TestNode");
        QCOMPARE(model.nodes().size(), 1);
        model.removeNode(id);
        QCOMPARE(model.nodes().size(), 0);
    }

    void testNodeData()
    {
        GraphModel model;
        QUuid id = model.addNode("TestNode");
        model.setNodeData(id, "value", 42);
        QCOMPARE(model.nodeData(id, "value"), QVariant(42));
        QVERIFY(model.nodeData(id, "nonexistent").isNull());
    }

    void testNodePosition()
    {
        GraphModel model;
        QUuid id = model.addNode("TestNode");
        model.setNodePosition(id, QPointF(50, 60));
        QCOMPARE(model.nodePosition(id), QPointF(50, 60));
    }

    void testConnectPorts()
    {
        GraphModel model;
        QUuid src = model.addNode("Source");
        QUuid dst = model.addNode("Dest");
        QUuid edge = model.connectPorts(src, "out", dst, "in");
        QVERIFY(!edge.isNull());
        QCOMPARE(model.edges().size(), 1);
    }

    void testDisconnectEdge()
    {
        GraphModel model;
        QUuid src = model.addNode("Source");
        QUuid dst = model.addNode("Dest");
        QUuid edge = model.connectPorts(src, "out", dst, "in");
        QCOMPARE(model.edges().size(), 1);
        model.disconnectEdge(edge);
        QCOMPARE(model.edges().size(), 0);
    }

    void testRemoveNodeRemovesEdges()
    {
        GraphModel model;
        QUuid src = model.addNode("Source");
        QUuid dst = model.addNode("Dest");
        model.connectPorts(src, "out", dst, "in");
        QCOMPARE(model.edges().size(), 1);
        model.removeNode(src);
        QCOMPARE(model.edges().size(), 0);
    }

    void testTopologicalSort()
    {
        GraphModel model;
        QUuid a = model.addNode("A");
        QUuid b = model.addNode("B");
        QUuid c = model.addNode("C");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", c, "in");

        auto sorted = model.topologicalSort();
        QCOMPARE(sorted.size(), 3);
        QCOMPARE(sorted[0], a);
        QCOMPARE(sorted[1], b);
        QCOMPARE(sorted[2], c);
    }

    void testCycleDetection()
    {
        GraphModel model;
        QUuid a = model.addNode("A");
        QUuid b = model.addNode("B");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", a, "in");
        QVERIFY(model.hasCycles());
    }

    void testNoCycles()
    {
        GraphModel model;
        model.addNode("A");
        model.addNode("B");
        QVERIFY(!model.hasCycles());
    }
};

QTEST_MAIN(TestGraphModel)
#include "tst_GraphModel.moc"
