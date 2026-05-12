import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import NodeEditor

Item {
    id: root

    property var graphModel: null
    property var undoManager: null
    property string nodeId: ""
    property var nodeInfo: ({})
    property bool selected: false
    property int _dataVersion: 0
    property int _connectionVersion: 0

    width: 180
    height: 30 + body.height

    readonly property var _displayTypes: [
        "output/display/number", "output/display/text", "output/display/boolean",
        "output/display/vector", "output/display/linePlot", "output/display/scatterPlot",
        "output/display/barChart", "output/display/pieChart", "output/display/histogram",
        "output/display/heatmap", "output/display/matrix", "output/display/memory",
        "output/display/progress", "output/display/gauge", "output/display/compass",
        "output/display/fps", "output/display/ledMatrix"
    ]

    property bool _isDisplayNode: false

    function findCanvas() {
        var p = root.parent
        while (p) {
            if (p.objectName && p.objectName.indexOf("NodeCanvas") === 0)
                return p
            p = p.parent
        }
        return null
    }

    function isSelected() {
        if (root.selected) return true
        var canvas = findCanvas()
        if (!canvas) return false
        return canvas.selectedNodeIds && canvas.selectedNodeIds.indexOf(root.nodeId) >= 0
    }

    function truncate(val) {
        if (val === undefined || val === null) return ""
        var s = String(val)
        if (s.length > 5) return s.substring(0, 5) + ".."
        return s
    }

    function syncFromModel() {
        if (!graphModel || !nodeId) return
        nodeInfo = graphModel.qmlNodeInfo(nodeId)
        if (nodeInfo && nodeInfo.x !== undefined) {
            root.x = nodeInfo.x
            root.y = nodeInfo.y
        }
        _isDisplayNode = nodeInfo && nodeInfo.type && (
            nodeInfo.type.indexOf("output/display/") === 0 ||
            nodeInfo.type.indexOf("output/visual/") === 0)
    }

    Component.onCompleted: syncFromModel()

    Connections {
        target: graphModel
        function onQmlNodePositionChanged(id) {
            if (id === root.nodeId && !dragArea.active) {
                var info = graphModel.qmlNodeInfo(root.nodeId)
                if (info && info.x !== undefined) {
                    root.x = info.x
                    root.y = info.y
                }
            }
        }
        function onQmlNodeDataChanged(id, key) {
            if (id === root.nodeId) {
                nodeInfo = graphModel.qmlNodeInfo(root.nodeId)
                _dataVersion++
            }
        }
        function onQmlNodePortsChanged(id) {
            if (id === root.nodeId) {
                nodeInfo = graphModel.qmlNodeInfo(root.nodeId)
                _dataVersion++
            }
        }
        function onQmlEdgeAdded(id) {
            if (!root.graphModel) return
            var info = root.graphModel.qmlEdgeInfo(id)
            if (info && (info.targetNodeId === root.nodeId || info.sourceNodeId === root.nodeId))
                _connectionVersion++
        }
        function onQmlEdgeRemoved(id) {
            // Edge is already gone, increment for all to be safe
            _connectionVersion++
        }
    }

    // Selection border (visible when selected)
    Rectangle {
        anchors.fill: parent
        anchors.margins: -3
        radius: 8
        color: "transparent"
        border.width: root.isSelected() ? 2 : 0
        border.color: "#00B4FF"
        z: -1
    }

    Rectangle {
        id: header
        width: parent.width
        height: 30
        radius: 6
        gradient: Gradient {
            GradientStop { position: 0.0; color: nodeInfo.color || "#4A9EFF" }
            GradientStop { position: 0.7; color: Qt.darker(nodeInfo.color || "#4A9EFF", 1.3) }
        }

        DragHandler {
            id: dragArea
            target: root
            cursorShape: Qt.OpenHandCursor

            property real dragStartX: 0
            property real dragStartY: 0

            onActiveChanged: {
                if (active) {
                    dragStartX = root.x
                    dragStartY = root.y
                    var canvas = findCanvas()
                    if (canvas) canvas.selectNode(root.nodeId)
                } else {
                    if (root.undoManager
                            && (Math.abs(root.x - dragStartX) > 1 || Math.abs(root.y - dragStartY) > 1)) {
                        // The node is already at the final position, but we tell undoManager about the move
                        // UndoManager's qmlMoveNode might try to set position again, which is fine
                        root.undoManager.qmlMoveNode(root.nodeId, dragStartX, dragStartY, root.x, root.y)
                    }
                }
            }

            onCentroidChanged: {
                if (active && graphModel) {
                    // Update graph model in real-time so connected edges can update their positions
                    graphModel.qmlSetNodePosition(root.nodeId, root.x, root.y)
                }
            }
        }

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            width: 4
            height: parent.height
            radius: 2
            color: Qt.lighter(nodeInfo.color || "#4A9EFF", 1.8)
        }

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: nodeInfo.type || "Node"
            color: "#FFFFFF"
            font.pixelSize: 12
            font.bold: true
        }
    }

    Rectangle {
        id: body
        anchors.top: header.bottom
        anchors.topMargin: -6
        width: parent.width
        height: inputColumn.height + outputColumn.height + (previewArea.visible ? previewArea.height : 0) + 24
        radius: 6
        color: "#2A2A2A"
        border.color: root.isSelected() ? "#00B4FF" : "#3A3A3A"
        border.width: root.isSelected() ? 2 : 1

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onClicked: function(mouse) {
                var canvas = findCanvas()
                if (!canvas) return
                if (mouse.modifiers & Qt.ControlModifier)
                    canvas.toggleNodeSelection(root.nodeId)
                else
                    canvas.selectNode(root.nodeId)
                mouse.accepted = false // Allow ports to see clicks too
            }
        }

        Column {
            id: inputColumn
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8
            spacing: 6

            Repeater {
                model: nodeInfo.inputPorts || []

                delegate: Item {
                    id: inputDelegate
                    width: inputColumn.width
                    height: isFilePathPort ? 24 : 16

                    property bool isFilePathPort: modelData === "filePath" && nodeInfo.type === "JsonInput"
                    property string dataOldVal: ""

                    // Hide the default port/field when using Browse button
                    Port {
                        id: portItem
                        graphModel: root.graphModel
                        nodeId: root.nodeId
                        portName: modelData
                        isInput: true
                        portType: (nodeInfo.inputPortTypes && nodeInfo.inputPortTypes[index]) || 0
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        visible: !isFilePathPort
                    }

                    Text {
                        text: modelData
                        color: "#CCCCCC"
                        font.pixelSize: 11
                        anchors.left: portItem.right
                        anchors.leftMargin: 6
                        anchors.verticalCenter: parent.verticalCenter
                        visible: !isFilePathPort
                    }

                    // Browse button for JsonInput filePath
                    Row {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 4
                        visible: isFilePathPort

                        Text {
                            id: filePathLabel
                            text: {
                                root._dataVersion
                                if (!root.graphModel) return "No file"
                                var val = root.graphModel.qmlNodeData(root.nodeId, "filePath")
                                if (val && val !== "") {
                                    var parts = String(val).split("/")
                                    return parts.length > 0 ? parts[parts.length - 1] : "No file"
                                }
                                return "No file"
                            }
                            color: filePathLabel.text === "No file" ? "#666666" : "#CCCCCC"
                            font.pixelSize: 10
                            elide: Text.ElideLeft
                            width: 80
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Button {
                            width: 24
                            height: 20
                            padding: 0
                            text: "..."
                            font.pixelSize: 10
                            onClicked: fileDialog.open()

                            FileDialog {
                                id: fileDialog
                                title: "Select JSON File"
                                nameFilters: ["JSON Files (*.json)", "All Files (*)"]
                                onAccepted: {
                                    var file = fileDialog.selectedFile.toString()
                                    file = file.replace(/^(file:\/{2})/, "")
                                    if (root.graphModel) {
                                        var oldVal = root.graphModel.qmlNodeData(root.nodeId, "filePath")
                                        if (root.undoManager)
                                            root.undoManager.qmlSetNodeData(root.nodeId, "filePath", oldVal, file)
                                        else
                                            root.graphModel.qmlSetNodeData(root.nodeId, "filePath", file)
                                    }
                                }
                            }
                        }
                    }

                    // Standard text field for normal input ports
                    Item {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        width: 70
                        height: 20
                        visible: !inputDelegate.isFilePathPort

                        TextField {
                            id: inputField
                            anchors.fill: parent
                            horizontalAlignment: TextInput.AlignRight
                            color: activeFocus ? "#FFFFFF" : "transparent"
                            background: Rectangle {
                                color: "#3A3A3A"
                                radius: 3
                                border.color: "#555555"
                                border.width: 1
                            }
                            font.pixelSize: 10
                            padding: 2

                            text: {
                                if (!root.graphModel) return ""
                                root._dataVersion
                                var val = root.graphModel.qmlNodeData(root.nodeId, modelData)
                                return val !== undefined && val !== null ? String(val) : ""
                            }

                            readOnly: {
                                root._connectionVersion
                                root.graphModel ? root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true) : false
                            }

                            onActiveFocusChanged: {
                                if (activeFocus)
                                    inputDelegate.dataOldVal = text
                            }

                            onEditingFinished: {
                                if (root.graphModel && root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true))
                                    return
                                if (root.undoManager && inputDelegate.dataOldVal !== text)
                                    root.undoManager.qmlSetNodeData(root.nodeId, modelData, inputDelegate.dataOldVal, text)
                            }
                        }

                        // Truncated display overlay (visible when not editing)
                        Text {
                            anchors.fill: parent
                            anchors.rightMargin: 4
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignVCenter
                            color: "#FFFFFF"
                            font.pixelSize: 10
                            text: root.truncate(inputField.text)
                            visible: !inputField.activeFocus
                            clip: true
                        }
                    }
                }
            }
        }

        Column {
            id: outputColumn
            anchors.top: inputColumn.bottom
            anchors.topMargin: inputColumn.count > 0 ? 8 : 0
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8
            spacing: 6

            Rectangle {
                height: 10
                width: parent.width
                color: "transparent" // or "#00000000"
            }

                Repeater {
                model: nodeInfo.outputPorts || []

                Item {
                    width: outputColumn.width
                    height: 16

                    Text {
                        text: modelData
                        color: "#CCCCCC"
                        font.pixelSize: 11
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                    }

                    Text {
                        text: {
                            if (!root.graphModel) return ""
                            root._dataVersion
                            var val = root.graphModel.qmlNodeData(root.nodeId, modelData)
                            return root.truncate(val)
                        }
                        color: "#4CDF8B"
                        font.pixelSize: 11
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: portItem.left
                        anchors.rightMargin: 6
                    }

                    Port {
                        id: portItem
                        graphModel: root.graphModel
                        nodeId: root.nodeId
                        portName: modelData
                        isInput: false
                        portType: (nodeInfo.outputPortTypes && nodeInfo.outputPortTypes[index]) || 0
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                    }
                }
            }
        }

        // ── Inline Preview for display nodes ──
        Item {
            id: previewArea
            anchors.top: outputColumn.bottom
            anchors.topMargin: 4
            anchors.left: parent.left
            anchors.leftMargin: 4
            anchors.right: parent.right
            anchors.rightMargin: 4
            visible: _isDisplayNode && graphModel
            height: visible ? previewLoader.height : 0
            clip: true

            Loader {
                id: previewLoader
                width: parent.width
                active: parent.visible
                sourceComponent: _isDisplayNode ? previewComponent : null
            }
        }

        Component {
            id: previewComponent

            Item {
                width: previewArea.width
                height: previewHeight()
                property string displayVal: graphModel ? String(graphModel.qmlNodeData(nodeId, "display") || "") : ""

                function previewHeight() {
                    var t = nodeInfo.type
                    if (t === "output/display/ledMatrix") return 100
                    if (t === "output/display/gauge") return 90
                    if (t === "output/display/compass") return 80
                    if (t === "output/display/boolean" || t === "output/display/progress") return 30
                    if (t === "output/display/text" || t === "output/display/memory" || t === "output/display/matrix") return 50
                    if (t === "output/display/number" || t === "output/display/vector" || t === "output/display/fps") return 40
                    return 90
                }

                function isImageType() {
                    var t = nodeInfo.type
                    return t === "output/display/ledMatrix" || t === "output/display/linePlot" ||
                           t === "output/display/scatterPlot" || t === "output/display/barChart" ||
                           t === "output/display/pieChart" || t === "output/display/histogram" ||
                           t === "output/display/heatmap" || t === "output/display/progress" ||
                           t === "output/display/gauge" || t === "output/display/compass" ||
                           t === "output/display/boolean" || t.indexOf("output/visual/") === 0
                }

                Rectangle {
                    anchors.fill: parent
                    color: "#1E1E1E"
                    radius: 4
                    border.color: "#3A3A3A"
                    border.width: 1

                    Image {
                        anchors.fill: parent
                        anchors.margins: 2
                        fillMode: Image.PreserveAspectFit
                        source: isImageType() && displayVal.length > 0
                            ? "data:image/png;base64," + displayVal
                            : ""
                        visible: isImageType() && displayVal.length > 0
                    }

                    Text {
                        anchors.fill: parent
                        anchors.margins: 4
                        color: "#CCCCCC"
                        font.pixelSize: 10
                        font.family: "monospace"
                        wrapMode: Text.Wrap
                        elide: Text.ElideRight
                        text: isImageType() ? "" : displayVal
                        visible: !isImageType() && displayVal.length > 0
                    }
                }

                Connections {
                    target: graphModel
                    function onQmlNodeDataChanged(id, key) {
                        if (id === nodeId)
                            displayVal = String(graphModel.qmlNodeData(nodeId, "display") || "")
                    }
                }
            }
        }
    }
}
