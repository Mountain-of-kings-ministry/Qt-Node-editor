#include <QtTest>
#include <QtTest/QtTest>
#include "NodeEditor/GraphModel.h"

using namespace NodeEditor;

class TestGraphModel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qRegisterMetaType<uint64_t>("uint64_t");
    }

    void testAddNode()
    {
        GraphModel model;
        NodeID id = model.addNode("TestNode", QPointF(100, 200));
        QVERIFY(id != 0);
        QCOMPARE(model.allGraphNodes().size(), (size_t)1);
        const auto *gn = model.graphNode(id);
        QVERIFY(gn != nullptr);
        QCOMPARE(gn->type, "TestNode");
        const auto *ui = model.nodeUIState(id);
        QVERIFY(ui != nullptr);
        QCOMPARE(ui->x, 100.0);
        QCOMPARE(ui->y, 200.0);
    }

    void testRemoveNode()
    {
        GraphModel model;
        NodeID id = model.addNode("TestNode");
        QCOMPARE(model.allGraphNodes().size(), (size_t)1);
        model.removeNode(id);
        QCOMPARE(model.allGraphNodes().size(), (size_t)0);
    }

    void testNodeData()
    {
        GraphModel model;
        NodeID id = model.addNode("TestNode");
        model.setNodeData(id, "value", 42);
        QCOMPARE(model.nodeData(id, "value"), QVariant(42));
        QVERIFY(model.nodeData(id, "nonexistent").isNull());
    }

    void testNodePosition()
    {
        GraphModel model;
        NodeID id = model.addNode("TestNode");
        model.setNodePosition(id, QPointF(50, 60));
        QCOMPARE(model.nodePosition(id), QPointF(50, 60));
    }

    void testConnectPorts()
    {
        GraphModel model;
        NodeID src = model.addNode("Source");
        NodeID dst = model.addNode("Dest");
        EdgeID edge = model.connectPorts(src, "out", dst, "in");
        QVERIFY(edge != 0);
        QCOMPARE(model.allGraphEdges().size(), (size_t)1);
    }

    void testDisconnectEdge()
    {
        GraphModel model;
        NodeID src = model.addNode("Source");
        NodeID dst = model.addNode("Dest");
        EdgeID edge = model.connectPorts(src, "out", dst, "in");
        QCOMPARE(model.allGraphEdges().size(), (size_t)1);
        model.disconnectEdge(edge);
        QCOMPARE(model.allGraphEdges().size(), (size_t)0);
    }

    void testRemoveNodeRemovesEdges()
    {
        GraphModel model;
        NodeID src = model.addNode("Source");
        NodeID dst = model.addNode("Dest");
        model.connectPorts(src, "out", dst, "in");
        QCOMPARE(model.allGraphEdges().size(), (size_t)1);
        model.removeNode(src);
        QCOMPARE(model.allGraphEdges().size(), (size_t)0);
    }

    void testTopologicalSort()
    {
        GraphModel model;
        NodeID a = model.addNode("A");
        NodeID b = model.addNode("B");
        NodeID c = model.addNode("C");
        model.connectPorts(a, "out", b, "in");
        model.connectPorts(b, "out", c, "in");

        const auto &sorted = model.cachedTopologicalOrder();
        QCOMPARE(sorted.size(), 3);
        QCOMPARE(sorted[0], a);
        QCOMPARE(sorted[1], b);
        QCOMPARE(sorted[2], c);
    }

    void testCycleDetection()
    {
        GraphModel model;
        NodeID a = model.addNode("A");
        NodeID b = model.addNode("B");
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
