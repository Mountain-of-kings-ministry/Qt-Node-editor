import QtQuick
import QtQuick.Shapes
import NodeEditor

Shape {
    id: root

    property var graphModel: null
    property var undoManager: null
    property string edgeId: ""
    property string sourceNodeId: ""
    property string sourcePort: ""
    property string targetNodeId: ""
    property string targetPort: ""

    property real sourceX: 0
    property real sourceY: 0
    property real targetX: 0
    property real targetY: 0

    property bool hovered: false

    width: 0
    height: 0

    function updatePositions() {
        if (!graphModel) return
        var srcInfo = graphModel.qmlNodeInfo(sourceNodeId)
        var tgtInfo = graphModel.qmlNodeInfo(targetNodeId)
        if (!srcInfo || !tgtInfo) return

        sourceX = srcInfo.x + 160
        sourceY = srcInfo.y + 30 + 7

        var outIdx = srcInfo.outputPorts ? srcInfo.outputPorts.indexOf(sourcePort) : -1
        if (outIdx >= 0)
            sourceY = srcInfo.y + 42 + outIdx * 22

        targetX = tgtInfo.x
        var inIdx = tgtInfo.inputPorts ? tgtInfo.inputPorts.indexOf(targetPort) : -1
        if (inIdx >= 0)
            targetY = tgtInfo.y + 42 + inIdx * 22
        else
            targetY = tgtInfo.y + 30 + 7
    }

    Connections {
        target: graphModel
        function onQmlNodePositionChanged(nodeId) {
            if (nodeId === sourceNodeId || nodeId === targetNodeId)
                root.updatePositions()
        }
    }

    Component.onCompleted: updatePositions()

    ShapePath {
        id: shapePath
        strokeColor: root.hovered ? "#00B4FF" : "#888888"
        strokeWidth: root.hovered ? 3 : 2
        fillColor: "transparent"
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin

        startX: root.sourceX
        startY: root.sourceY

        PathCubic {
            control1X: root.sourceX + Math.max(60, Math.abs(root.targetX - root.sourceX) * 0.5)
            control1Y: root.sourceY
            control2X: root.targetX - Math.max(60, Math.abs(root.targetX - root.sourceX) * 0.5)
            control2Y: root.targetY
            x: root.targetX
            y: root.targetY
        }
    }

    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onEntered: root.hovered = true
        onExited: root.hovered = false
        onClicked: {
            if (mouse.button === Qt.RightButton) {
                if (root.undoManager)
                    root.undoManager.qmlDisconnectEdge(root.edgeId)
                else if (root.graphModel)
                    root.graphModel.qmlDisconnectEdge(root.edgeId)
            }
        }
    }
}
