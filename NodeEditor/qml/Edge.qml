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

    property real sourceYOffset: 0
    property real targetYOffset: 0

    property bool hovered: false

    width: 0
    height: 0

    // Optimize: Use CurveRenderer for high quality curves (Qt 6.6+)
    preferredRendererType: Shape.CurveRenderer

    function getPortYOffset(nodeId, portName, isOutput) {
        if (!graphModel) return 0
        var info = graphModel.qmlNodeInfo(nodeId)
        if (!info) return 0
        
        if (isOutput) {
            var ports = info.outputPorts || []
            var idx = ports.indexOf(portName)
            if (idx < 0) return 42
            var numInputs = (info.inputPorts || []).length
            if (numInputs > 0)
                return 44 + numInputs * 22 + idx * 22
            else
                return 42 + idx * 22
        } else {
            var ports2 = info.inputPorts || []
            var idx2 = ports2.indexOf(portName)
            if (idx2 < 0) return 42
            return 42 + idx2 * 22
        }
    }

    function updateYOffsets() {
        sourceYOffset = getPortYOffset(sourceNodeId, sourcePort, true)
        targetYOffset = getPortYOffset(targetNodeId, targetPort, false)
    }

    function updatePositions() {
        if (!graphModel) return
        var srcPos = graphModel.qmlNodePosition(sourceNodeId)
        var tgtPos = graphModel.qmlNodePosition(targetNodeId)
        
        sourceX = srcPos.x + 165
        sourceY = srcPos.y + sourceYOffset

        targetX = tgtPos.x + 15
        targetY = tgtPos.y + targetYOffset
    }

    readonly property real curveDX: Math.max(60, Math.abs(targetX - sourceX) * 0.5)

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
        function onQmlNodePortsChanged(nodeId) {
            if (nodeId === sourceNodeId || nodeId === targetNodeId) {
                root.updateYOffsets()
                root.updatePositions()
            }
        }
    }

    Component.onCompleted: {
        updateYOffsets()
        updatePositions()
    }

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
            control1X: root.sourceX + root.curveDX
            control1Y: root.sourceY
            control2X: root.targetX - root.curveDX
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

    // Invisible wider hit area for mouse interaction - now follows the curve!
    ShapePath {
        strokeColor: "transparent"
        strokeWidth: 16
        fillColor: "transparent"

        startX: root.sourceX
        startY: root.sourceY

        PathCubic {
            control1X: root.sourceX + root.curveDX
            control1Y: root.sourceY
            control2X: root.targetX - root.curveDX
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
