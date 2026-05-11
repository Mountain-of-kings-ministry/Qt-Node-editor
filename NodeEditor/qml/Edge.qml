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

    function portCenterY(info, portName, isOutput) {
        if (isOutput) {
            var ports = info.outputPorts || []
            var idx = ports.indexOf(portName)
            if (idx < 0) return info.y + 42
            var numInputs = (info.inputPorts || []).length
            if (numInputs > 0)
                return info.y + 44 + numInputs * 22 + idx * 22
            else
                return info.y + 42 + idx * 22
        } else {
            var ports2 = info.inputPorts || []
            var idx2 = ports2.indexOf(portName)
            if (idx2 < 0) return info.y + 42
            return info.y + 42 + idx2 * 22
        }
    }

    function updatePositions() {
        if (!graphModel) return
        var srcInfo = graphModel.qmlNodeInfo(sourceNodeId)
        var tgtInfo = graphModel.qmlNodeInfo(targetNodeId)
        if (!srcInfo || !tgtInfo) return

        sourceX = srcInfo.x + 165
        sourceY = root.portCenterY(srcInfo, sourcePort, true)

        targetX = tgtInfo.x + 15
        targetY = root.portCenterY(tgtInfo, targetPort, false)
    }

    function curveDX() {
        return Math.max(60, Math.abs(targetX - sourceX) * 0.5)
    }

    // Approximate angle from source to target (good enough for arrow head)
    function angle() {
        return Math.atan2(targetY - sourceY, targetX - sourceX)
    }

    Connections {
        target: graphModel
        function onQmlNodePositionChanged(nodeId) {
            if (nodeId === sourceNodeId || nodeId === targetNodeId)
                root.updatePositions()
        }
    }

    Component.onCompleted: updatePositions()

    // Bézier curve line
    ShapePath {
        strokeColor: root.hovered ? "#00B4FF" : "#888888"
        strokeWidth: root.hovered ? 3 : 2
        fillColor: "transparent"
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin

        startX: root.sourceX
        startY: root.sourceY

        PathCubic {
            control1X: root.sourceX + root.curveDX()
            control1Y: root.sourceY
            control2X: root.targetX - root.curveDX()
            control2Y: root.targetY
            x: root.targetX
            y: root.targetY
        }
    }

    // Arrow head (filled triangle at target end)
    ShapePath {
        strokeColor: root.hovered ? "#00B4FF" : "#888888"
        strokeWidth: 1
        fillColor: root.hovered ? "#00B4FF" : "#888888"
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin

        startX: root.targetX
        startY: root.targetY

        PathLine {
            x: root.targetX - Math.cos(root.angle() - 0.4) * 10
            y: root.targetY - Math.sin(root.angle() - 0.4) * 10
        }
        PathLine {
            x: root.targetX - Math.cos(root.angle() + 0.4) * 10
            y: root.targetY - Math.sin(root.angle() + 0.4) * 10
        }
        PathLine {
            x: root.targetX
            y: root.targetY
        }
    }

    // Invisible wider hit area for mouse interaction
    ShapePath {
        strokeColor: "transparent"
        strokeWidth: 14
        fillColor: "transparent"

        startX: root.sourceX
        startY: root.sourceY

        PathLine {
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
