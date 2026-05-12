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
        
        var inPorts = info.inputPorts || []
        if (!isOutput) {
            var idx = inPorts.indexOf(portName)
            // Input column starts at y=34 (24+10). Port center is at +8.
            return idx < 0 ? 42 : 42 + idx * 22
        }
        
        var outPorts = info.outputPorts || []
        var idxOut = outPorts.indexOf(portName)
        if (idxOut < 0) return 42
        
        var numIn = inPorts.length
        // Output column positioning logic:
        // if numIn > 0: top = 36 + 22*numIn. Spacer = 10. Center = top + 10 + 8 = 54 + 22*numIn.
        // if numIn == 0: top = 34. Spacer = 10. Center = top + 10 + 8 = 52.
        if (numIn === 0)
            return 52 + idxOut * 22
        return 54 + numIn * 22 + idxOut * 22
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

    readonly property real curveDX: Math.max(80, Math.abs(targetX - sourceX) * 0.45)

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

    readonly property color portColor: graphModel ? graphModel.portTypeColor(graphModel.qmlPortType(sourceNodeId, sourcePort, false)) : "#888888"

    // Bézier curve line
    ShapePath {
        strokeColor: root.hovered ? Qt.lighter(root.portColor, 1.3) : root.portColor
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

    // Dot at target end (replaces arrowhead)
    ShapePath {
        strokeColor: "transparent"
        fillColor: root.hovered ? Qt.lighter(root.portColor, 1.3) : root.portColor
        
        // Start at target end
        startX: root.targetX - 6
        startY: root.targetY

        PathArc {
            x: root.targetX + 6
            y: root.targetY
            radiusX: 6
            radiusY: 6
            useLargeArc: true
        }
        PathArc {
            x: root.targetX - 6
            y: root.targetY
            radiusX: 6
            radiusY: 6
            useLargeArc: true
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
        acceptedButtons: Qt.LeftButton

        onEntered: root.hovered = true
        onExited: root.hovered = false
    }
}
