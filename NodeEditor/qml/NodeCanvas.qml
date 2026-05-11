import QtQuick
import QtQuick.Shapes
import NodeEditor

Item {
    id: root

    objectName: "NodeCanvas"

    property var graphModel: null
    property var undoManager: null

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

    property string selectedNodeId: ""
    property var selectedNodeIds: []
    property string selectMode: "default"

    signal nodeSelected(string nodeId)
    signal nodeDeselected()

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
        root.zoom = Math.min(scaleX, scaleY)
        root.zoom = Math.max(0.1, Math.min(5, root.zoom))
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
                for (var x = ox; x < width; x += gs * root.zoom) {
                    ctx.moveTo(x + 0.5, 0)
                    ctx.lineTo(x + 0.5, height)
                }
                for (var y = oy; y < height; y += gs * root.zoom) {
                    ctx.moveTo(0, y + 0.5)
                    ctx.lineTo(width, y + 0.5)
                }
                ctx.stroke()
            }
        }
    }

    onGraphModelChanged: {
        rebuildNodeList()
        rebuildEdgeList()
    }

    // Synchronize node/edge lists with GraphModel signals
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

    // World container
    Item {
        id: world
        x: root.panX
        y: root.panY
        transform: Scale { origin.x: 0; origin.y: 0; xScale: root.zoom; yScale: root.zoom }

        Instantiator {
            id: nodeInstantiator
            active: root.graphModel !== null
            model: nodeModel

            delegate: Node {
                graphModel: root.graphModel
                undoManager: root.undoManager
                nodeId: model.nodeId
                selected: root.selectedNodeIds.indexOf(model.nodeId) >= 0
            }

            onObjectAdded: function(index, obj) { obj.parent = world }
        }

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
        property real boxStartX: 0
        property real boxStartY: 0
    }

    // Rubber band for connection creation
    Canvas {
        id: rubberBand
        visible: root.connecting
        anchors.fill: parent
        z: 1000

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.strokeStyle = "#00B4FF"
            ctx.lineWidth = 2
            ctx.beginPath()
            ctx.moveTo(root.connectStartX, root.connectStartY)
            var dx = Math.max(60, Math.abs(root.connectCurrentX - root.connectStartX) * 0.5)
            ctx.bezierCurveTo(
                root.connectStartX + dx, root.connectStartY,
                root.connectCurrentX - dx, root.connectCurrentY,
                root.connectCurrentX, root.connectCurrentY)
            ctx.stroke()
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

    // Interaction: pan + zoom + selection
    MouseArea {
        id: canvasMouse
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton | Qt.RightButton | Qt.LeftButton
        propagateComposedEvents: true

        property real lastPanX: 0
        property real lastPanY: 0
        property real panStartX: 0
        property real panStartY: 0
        property bool isBoxSelecting: false

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
            if (mouse.button === Qt.LeftButton && root.selectMode === "box" && !root.connecting) {
                isBoxSelecting = true
                boxSelectRect.boxStartX = mouse.x
                boxSelectRect.boxStartY = mouse.y
                boxSelectRect.x = mouse.x
                boxSelectRect.y = mouse.y
                boxSelectRect.width = 0
                boxSelectRect.height = 0
                boxSelectRect.boxSelectActive = true
                return
            }
            if (mouse.button === Qt.LeftButton && !root.connecting) {
                if (root.hitTestNode(mouse.x, mouse.y) === "" && !(mouse.modifiers & Qt.ControlModifier))
                    root.clearSelection()
                return
            }
            if (mouse.button === Qt.MiddleButton || mouse.button === Qt.RightButton) {
                lastPanX = root.panX
                lastPanY = root.panY
                panStartX = mouse.x
                panStartY = mouse.y
            }
        }

        onPositionChanged: function(mouse) {
            if (pressed && (mouse.button === Qt.MiddleButton || mouse.button === Qt.RightButton)) {
                root.panX = lastPanX + (mouse.x - panStartX)
                root.panY = lastPanY + (mouse.y - panStartY)
                gridCanvas.requestPaint()
            }
            if (pressed && isBoxSelecting && mouse.button === Qt.LeftButton) {
                boxSelectRect.x = Math.min(boxSelectRect.boxStartX, mouse.x)
                boxSelectRect.y = Math.min(boxSelectRect.boxStartY, mouse.y)
                boxSelectRect.width = Math.abs(mouse.x - boxSelectRect.boxStartX)
                boxSelectRect.height = Math.abs(mouse.y - boxSelectRect.boxStartY)
            }
            if (root.connecting) {
                root.connectCurrentX = mouse.x
                root.connectCurrentY = mouse.y
                rubberBand.requestPaint()
            }
        }

        onReleased: function(mouse) {
            if (isBoxSelecting && mouse.button === Qt.LeftButton) {
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
        }
    }

    function startConnection(nodeId, portName, screenX, screenY) {
        connecting = true
        connectSourceNodeId = nodeId
        connectSourcePort = portName
        connectStartX = screenX
        connectStartY = screenY
        connectCurrentX = screenX
        connectCurrentY = screenY
    }

    function endConnection(nodeId, portName) {
        if (!connecting) return
        connecting = false
        if (connectSourceNodeId !== nodeId && graphModel) {
            if (root.undoManager)
                root.undoManager.qmlConnectPorts(connectSourceNodeId, connectSourcePort, nodeId, portName)
            else
                graphModel.qmlConnectPorts(connectSourceNodeId, connectSourcePort, nodeId, portName)
        }
    }

    onPanXChanged: gridCanvas.requestPaint()
    onPanYChanged: gridCanvas.requestPaint()
    onZoomChanged: gridCanvas.requestPaint()
}
