import QtQuick
import QtQuick.Shapes
import NodeEditor

Item {
    id: root

    objectName: "NodeCanvas"

    property var graphModel: null
    property var undoManager: null
    property var previewManager: null

    property alias world: world

    property real zoom: 1.0
    property real panX: 0
    property real panY: 0

    property bool connecting: false
    property string connectSourceNodeId: ""
    property string connectSourcePort: ""
    property real connectStartX: 0
    property real connectStartY: 0
    property real connectCurrentX: 0
    property real connectCurrentY: 0

    onConnectCurrentXChanged: rubberBand.requestPaint()
    onConnectCurrentYChanged: rubberBand.requestPaint()

    property string selectedNodeId: ""
    property var selectedNodeIds: []
    property string selectMode: "default"

    signal nodeSelected(string nodeId)
    signal nodeDeselected()
    signal interactionStarted()
    signal interactionEnded()

    QtObject {
        id: nodeLayout
        function getPortYOffset(nodeId, portName, isInput) {
            if (!graphModel) return 42
            var info = graphModel.qmlNodeInfo(nodeId)
            if (!info) return 42
            
            var inPorts = info.inputPorts || []
            if (isInput) {
                var idx = inPorts.indexOf(portName)
                return idx < 0 ? 42 : 42 + idx * 22
            }
            
            var outPorts = info.outputPorts || []
            var idxOut = outPorts.indexOf(portName)
            if (idxOut < 0) return 42
            
            var numIn = inPorts.length
            if (numIn === 0)
                return 52 + idxOut * 22
            return 54 + numIn * 22 + idxOut * 22
        }

        function getPortWorldPos(nodeId, portName, isInput) {
            var pos = graphModel ? graphModel.qmlNodePosition(nodeId) : Qt.point(0,0)
            return {
                x: pos.x + (isInput ? 15 : 165),
                y: pos.y + getPortYOffset(nodeId, portName, isInput)
            }
        }
    }

    function screenToWorld(sx, sy) {
        return {
            x: (sx - root.panX) / root.zoom,
            y: (sy - root.panY) / root.zoom
        }
    }

    function selectNode(nodeId) {
        selectedNodeIds = [nodeId]
        selectedNodeId = nodeId
        nodeSelected(nodeId)
    }

    function clearSelection() {
        selectedNodeIds = []
        selectedNodeId = ""
        nodeDeselected()
    }

    function toggleNodeSelection(nodeId) {
        var idx = selectedNodeIds.indexOf(nodeId)
        if (idx >= 0) {
            selectedNodeIds.splice(idx, 1)
            if (selectedNodeIds.length > 0) {
                selectedNodeId = selectedNodeIds[selectedNodeIds.length - 1]
                nodeSelected(selectedNodeId)
            } else {
                selectedNodeId = ""
                nodeDeselected()
            }
        } else {
            selectedNodeIds.push(nodeId)
            selectedNodeId = nodeId
            nodeSelected(nodeId)
        }
    }

    function nodesInRect(worldX1, worldY1, worldX2, worldY2) {
        var found = []
        if (!graphModel) return found
        var ids = graphModel.qmlNodeIds()
        var minX = Math.min(worldX1, worldX2)
        var maxX = Math.max(worldX1, worldX2)
        var minY = Math.min(worldY1, worldY2)
        var maxY = Math.max(worldY1, worldY2)
        for (var i = 0; i < ids.length; i++) {
            var info = graphModel.qmlNodeInfo(ids[i])
            if (!info) continue
            var cx = info.x || 0
            var cy = info.y || 0
            if (cx >= minX && cx <= maxX && cy >= minY && cy <= maxY)
                found.push(ids[i])
        }
        return found
    }

    function hitTestNode(screenX, screenY) {
        if (!root.graphModel) return ""
        var wp = root.screenToWorld(screenX, screenY)
        var ids = root.graphModel.qmlNodeIds()
        for (var i = 0; i < ids.length; i++) {
            var info = root.graphModel.qmlNodeInfo(ids[i])
            if (!info) continue
            var nx = info.x || 0
            var ny = info.y || 0
            if (wp.x >= nx && wp.x <= nx + 180 && wp.y >= ny && wp.y <= ny + 120)
                return ids[i]
        }
        return ""
    }

    function fitToView() {
        if (!graphModel) return
        var ids = graphModel.qmlNodeIds()
        if (ids.length === 0) return
        var minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity
        for (var i = 0; i < ids.length; i++) {
            var info = graphModel.qmlNodeInfo(ids[i])
            if (!info) continue
            var nx = info.x || 0
            var ny = info.y || 0
            if (nx < minX) minX = nx
            if (ny < minY) minY = ny
            if (nx + 180 > maxX) maxX = nx + 180
            if (ny + 120 > maxY) maxY = ny + 120
        }
        var margin = 40
        var contentW = (maxX - minX) + margin * 2
        var contentH = (maxY - minY) + margin * 2
        if (contentW <= 0 || contentH <= 0) return
        var scaleX = root.width / contentW
        var scaleY = root.height / contentH
        root.zoom = Math.max(0.1, Math.min(5, Math.min(scaleX, scaleY)))
        root.panX = -minX * root.zoom + margin * root.zoom
        root.panY = -minY * root.zoom + margin * root.zoom
        gridCanvas.requestPaint()
    }

    function fitToNode(nodeId) {
        if (!graphModel) return
        var info = graphModel.qmlNodeInfo(nodeId)
        if (!info) return
        root.zoom = 1.5
        root.panX = -(info.x || 0) * root.zoom + root.width / 2 - 90
        root.panY = -(info.y || 0) * root.zoom + root.height / 2 - 60
        gridCanvas.requestPaint()
    }

    // Grid background
    Rectangle {
        anchors.fill: parent
        color: "#1E1E1E"

        Canvas {
            id: gridCanvas
            anchors.fill: parent

            function gridSize() {
                var base = 20
                while (base * root.zoom < 8) base *= 2
                while (base * root.zoom > 40) base /= 2
                return Math.max(8, base)
            }

            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()
                ctx.lineWidth = 1
                ctx.strokeStyle = "#333333"
                var gs = gridSize()
                var ox = root.panX % (gs * root.zoom)
                var oy = root.panY % (gs * root.zoom)
                if (ox > 0) ox -= gs * root.zoom
                if (oy > 0) oy -= gs * root.zoom
                ctx.beginPath()
                for (var x = ox; x < width; x += gs * root.zoom)
                    { ctx.moveTo(x + 0.5, 0); ctx.lineTo(x + 0.5, height) }
                for (var y = oy; y < height; y += gs * root.zoom)
                    { ctx.moveTo(0, y + 0.5); ctx.lineTo(width, y + 0.5) }
                ctx.stroke()
            }
        }
    }

    onGraphModelChanged: {
        rebuildNodeList()
        rebuildEdgeList()
    }

    Connections {
        target: root.graphModel
        function onQmlNodeAdded() { rebuildNodeList() }
        function onQmlNodeRemoved() { rebuildNodeList() }
        function onQmlEdgeAdded() { rebuildEdgeList() }
        function onQmlEdgeRemoved() { rebuildEdgeList() }
    }

    function rebuildNodeList() {
        nodeModel.clear()
        if (!root.graphModel) return
        var ids = root.graphModel.qmlNodeIds()
        for (var i = 0; i < ids.length; i++)
            nodeModel.append({ nodeId: ids[i] })
    }

    function rebuildEdgeList() {
        edgeModel.clear()
        if (!root.graphModel) return
        var ids = root.graphModel.qmlEdgeIds()
        for (var i = 0; i < ids.length; i++)
            edgeModel.append({ edgeId: ids[i] })
    }

    ListModel { id: nodeModel }
    ListModel { id: edgeModel }

    // World container — z:10 so nodes sit above all interaction overlays
    Item {
        id: world
        z: 10
        x: root.panX
        y: root.panY
        scale: root.zoom
        transformOrigin: Item.TopLeft

        Instantiator {
            id: edgeInstantiator
            active: root.graphModel !== null
            model: edgeModel

            delegate: Edge {
                graphModel: root.graphModel
                undoManager: root.undoManager
                edgeId: model.edgeId
                sourceNodeId: graphModel ? graphModel.qmlEdgeInfo(model.edgeId).sourceNodeId : ""
                sourcePort: graphModel ? graphModel.qmlEdgeInfo(model.edgeId).sourcePort : ""
                targetNodeId: graphModel ? graphModel.qmlEdgeInfo(model.edgeId).targetNodeId : ""
                targetPort: graphModel ? graphModel.qmlEdgeInfo(model.edgeId).targetPort : ""
            }

            onObjectAdded: function(index, obj) { obj.parent = world }
        }

        Instantiator {
            id: nodeInstantiator
            active: root.graphModel !== null
            model: nodeModel

            delegate: Node {
                graphModel: root.graphModel
                undoManager: root.undoManager
                previewManager: root.previewManager
                nodeId: model.nodeId
                selected: root.selectedNodeIds.indexOf(model.nodeId) >= 0
            }

            onObjectAdded: function(index, obj) { obj.parent = world }
        }
    }

    // ── Interaction overlay (handles panning, selection, connections) ──
    MouseArea {
        id: mainMouseArea
        anchors.fill: parent
        z: 5
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton

        property bool isPanning: false
        property bool isBoxSelecting: false
        property real lastPanX: 0
        property real lastPanY: 0
        property real startX: 0
        property real startY: 0

        onWheel: function(wheel) {
            var oldZoom = root.zoom
            var factor = wheel.angleDelta.y > 0 ? 1.15 : 1 / 1.15
            root.zoom = Math.max(0.1, Math.min(5, root.zoom * factor))
            root.panX = wheel.x - (wheel.x - root.panX) * (root.zoom / oldZoom)
            root.panY = wheel.y - (wheel.y - root.panY) * (root.zoom / oldZoom)
            gridCanvas.requestPaint()
        }

        onPressed: function(mouse) {
            root.forceActiveFocus()
            startX = mouse.x
            startY = mouse.y
            lastPanX = root.panX
            lastPanY = root.panY

            if (root.connecting) return

            // Panning: Middle, Right, or Left (in default mode if no node hit)
            var hitNodeId = root.hitTestNode(mouse.x, mouse.y)
            if (mouse.button === Qt.MiddleButton || mouse.button === Qt.RightButton || 
                (mouse.button === Qt.LeftButton && root.selectMode === "default" && hitNodeId === "")) {
                isPanning = true
                cursorShape = Qt.ClosedHandCursor
                root.interactionStarted()
            } else if (mouse.button === Qt.LeftButton && root.selectMode === "box") {
                isBoxSelecting = true
                boxSelectRect.x = mouse.x
                boxSelectRect.y = mouse.y
                boxSelectRect.width = 0
                boxSelectRect.height = 0
                boxSelectRect.boxSelectActive = true
            }

            if (mouse.button === Qt.LeftButton && hitNodeId === "" && !(mouse.modifiers & Qt.ControlModifier)) {
                root.clearSelection()
            }
        }

        onPositionChanged: function(mouse) {
            if (isPanning) {
                root.panX = lastPanX + (mouse.x - startX)
                root.panY = lastPanY + (mouse.y - startY)
                gridCanvas.requestPaint()
            } else if (isBoxSelecting) {
                boxSelectRect.x = Math.min(startX, mouse.x)
                boxSelectRect.y = Math.min(startY, mouse.y)
                boxSelectRect.width = Math.abs(mouse.x - startX)
                boxSelectRect.height = Math.abs(mouse.y - startY)
            }

            if (root.connecting) {
                var worldPt = root.screenToWorld(mouse.x, mouse.y)
                root.connectCurrentX = worldPt.x
                root.connectCurrentY = worldPt.y
                 rubberBand.requestPaint()
            }
        }

        onReleased: function(mouse) {
            if (isPanning) {
                isPanning = false
                cursorShape = Qt.ArrowCursor
                root.interactionEnded()
            } else if (isBoxSelecting) {
                isBoxSelecting = false
                boxSelectRect.boxSelectActive = false
                if (boxSelectRect.width > 5 || boxSelectRect.height > 5) {
                    var topLeft = root.screenToWorld(boxSelectRect.x, boxSelectRect.y)
                    var bottomRight = root.screenToWorld(
                        boxSelectRect.x + boxSelectRect.width,
                        boxSelectRect.y + boxSelectRect.height)
                    var found = root.nodesInRect(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y)
                    if (found.length > 0) {
                        root.selectedNodeIds = found
                        root.selectedNodeId = found[found.length - 1]
                        nodeSelected(root.selectedNodeId)
                    }
                }
            }

            if (root.connecting) {
                root.cancelConnection()
            }
        }
    }

    // Box selection overlay
    Rectangle {
        id: boxSelectRect
        visible: boxSelectActive && root.selectMode === "box"
        color: "#1A4A9EFF"
        border.color: "#4A9EFF"
        border.width: 1
        z: 999

        property bool boxSelectActive: false
    }

    property color connectColor: "#00B4FF"
    onConnectingChanged: {
        if (connecting && graphModel) {
            var info = graphModel.qmlNodeInfo(connectSourceNodeId)
            var isInput = (info.inputPorts || []).indexOf(connectSourcePort) >= 0
            var type = graphModel.qmlPortType(connectSourceNodeId, connectSourcePort, isInput)
            connectColor = graphModel.portTypeColor(type)
        }
        rubberBand.requestPaint()
    }

    // Rubber band for connection creation (bezier curve + dots at both ends)
    Canvas {
        id: rubberBand
        visible: root.connecting
        anchors.fill: parent
        z: 1000

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            // Map world coordinates to screen space
            // World coordinate (wx, wy) is at screen (panX + wx * zoom, panY + wy * zoom)
            var sx = root.panX + root.connectStartX * root.zoom
            var sy = root.panY + root.connectStartY * root.zoom
            var ex = root.panX + root.connectCurrentX * root.zoom
            var ey = root.panY + root.connectCurrentY * root.zoom

            var dx = Math.max(80 * root.zoom, Math.abs(ex - sx) * 0.45)

            // Bezier curve
            ctx.strokeStyle = root.connectColor
            ctx.lineWidth = 2 * root.zoom
            ctx.lineCap = "round"
            ctx.beginPath()
            ctx.moveTo(sx, sy)
            ctx.bezierCurveTo(sx + dx, sy, ex - dx, ey, ex, ey)
            ctx.stroke()

            // Dots at both ends
            ctx.fillStyle = root.connectColor
            var r = 6 * root.zoom
            
            // Start dot
            ctx.beginPath()
            ctx.arc(sx, sy, r, 0, Math.PI * 2)
            ctx.fill()

            // End dot
            ctx.beginPath()
            ctx.arc(ex, ey, r, 0, Math.PI * 2)
            ctx.fill()
        }
    }

    // Drop area for palette drag-and-drop
    DropArea {
        anchors.fill: parent
        keys: ["nodeeditor/nodetype"]

        onDropped: function(drop) {
            var nodeType = drop.getDataAsString("nodeeditor/nodetype")
            if (!nodeType) return
            var pos = root.screenToWorld(drop.x, drop.y)
            if (root.undoManager)
                root.undoManager.qmlAddNode(nodeType, pos.x, pos.y)
            else
                root.graphModel.qmlAddNode(nodeType, pos.x, pos.y)
            drop.accept()
        }
    }

    function startConnection(nodeId, portName, screenX, screenY) {
        connecting = true
        connectSourceNodeId = nodeId
        connectSourcePort = portName
        
        // Use the shared layout logic to get the exact world center
        var isInput = (graphModel.qmlNodeInfo(nodeId).inputPorts || []).indexOf(portName) >= 0
        var wp = nodeLayout.getPortWorldPos(nodeId, portName, isInput)
        connectStartX = wp.x
        connectStartY = wp.y
        connectCurrentX = wp.x
        connectCurrentY = wp.y
    }

    function endConnection(nodeId, portName) {
        if (!connecting) return
        connecting = false
        if (connectSourceNodeId !== nodeId && graphModel) {
            // Determine which is input and which is output
            var srcIsInput = graphModel.qmlNodeInfo(connectSourceNodeId).inputPorts.indexOf(connectSourcePort) >= 0
            var tgtIsInput = graphModel.qmlNodeInfo(nodeId).inputPorts.indexOf(portName) >= 0
            
            if (srcIsInput !== tgtIsInput) {
                var outputNode = srcIsInput ? nodeId : connectSourceNodeId
                var outputPort = srcIsInput ? portName : connectSourcePort
                var inputNode = srcIsInput ? connectSourceNodeId : nodeId
                var inputPort = srcIsInput ? connectSourcePort : portName
                
                if (root.undoManager)
                    root.undoManager.qmlConnectPorts(outputNode, outputPort, inputNode, inputPort)
                else
                    graphModel.qmlConnectPorts(outputNode, outputPort, inputNode, inputPort)
            }
        }
    }

    function cancelConnection() {
        connecting = false
        connectSourceNodeId = ""
        connectSourcePort = ""
        rubberBand.requestPaint()
    }

    // Find the nearest input port to a world-space point (for connection completion)
    function findInputPortAt(worldX, worldY) {
        if (!graphModel) return null
        var ids = graphModel.qmlNodeIds()
        var closest = null
        var closestDist = 25
        for (var i = 0; i < ids.length; i++) {
            var info = graphModel.qmlNodeInfo(ids[i])
            if (!info) continue
            var ports = info.inputPorts || []
            var nx = info.x || 0
            var ny = info.y || 0
            for (var j = 0; j < ports.length; j++) {
                // Input ports sit at x+15, y+42 + idx*22 (matching Edge.qml corrected math)
                var px = nx + 15
                var py = ny + 42 + j * 22
                var dx = worldX - px
                var dy = worldY - py
                var d = Math.sqrt(dx * dx + dy * dy)
                if (d < closestDist) {
                    closestDist = d
                    closest = { nodeId: ids[i], portName: ports[j] }
                }
            }
        }
        return closest
    }

    onPanXChanged: { gridCanvas.requestPaint(); rubberBand.requestPaint() }
    onPanYChanged: { gridCanvas.requestPaint(); rubberBand.requestPaint() }
    onZoomChanged: { gridCanvas.requestPaint(); rubberBand.requestPaint() }
}
