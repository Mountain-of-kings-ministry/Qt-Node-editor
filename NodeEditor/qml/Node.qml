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

    function randomColor() {
        return Qt.hsva(Math.random(), 0.6 + Math.random() * 0.4, 0.7 + Math.random() * 0.3, 1.0)
    }

    function colorToComponents(color) {
        if (color === undefined || color === null || color === "")
            return Qt.vector4d(0, 0, 0, 1)
        if (color.r !== undefined)
            return Qt.vector4d(color.r, color.g, color.b, color.a !== undefined ? color.a : 1.0)
        var hex = String(color).replace("#", "")
        var r = parseInt(hex.substring(0, 2), 16) / 255
        var g = parseInt(hex.substring(2, 4), 16) / 255
        var b = parseInt(hex.substring(4, 6), 16) / 255
        return Qt.vector4d(r, g, b, hex.length >= 8 ? parseInt(hex.substring(6, 8), 16) / 255 : 1)
    }

    function colorToHsva(color) {
        var c = colorToComponents(color)
        var r = c.x, g = c.y, b = c.z
        var max = Math.max(r, g, b), min = Math.min(r, g, b)
        var h, s, v = max
        var d = max - min
        s = max === 0 ? 0 : d / max
        if (max === min) {
            h = 0
        } else {
            if (max === r) h = (g - b) / d + (g < b ? 6 : 0)
            else if (max === g) h = (b - r) / d + 2
            else h = (r - g) / d + 4
            h /= 6
        }
        return Qt.vector4d(h, s, v, c.w)
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
                    height: isFilePathPort ? 24 : (isColorPort ? 22 : 16)

                    property bool isFilePathPort: modelData === "filePath" || modelData === "path" || modelData === "folderPath"
                    property bool isColorPort: nodeInfo.inputPortTypes && index < nodeInfo.inputPortTypes.length && nodeInfo.inputPortTypes[index] === 4
                    property bool isFileWritePort: isFilePathPort && nodeInfo.type && (
                        nodeInfo.type.indexOf("/write") >= 0 || nodeInfo.type.indexOf("/fileWriter") >= 0
                    )
                    property bool isFolderSelectorPort: isFilePathPort && nodeInfo.type === "system/file/folderSelector"
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

                    // Browse button for file path ports
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
                                var val = root.graphModel.qmlNodeData(root.nodeId, modelData)
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
                            onClicked: {
                                if (isFolderSelectorPort)
                                    folderDialog.open()
                                else
                                    fileDialog.open()
                            }

                            FileDialog {
                                id: fileDialog
                                title: isFileWritePort ? "Select output file" : "Select file"
                                fileMode: isFileWritePort ? FileDialog.SaveFile : FileDialog.OpenFile
                                nameFilters: modelData === "filePath"
                                    ? ["JSON Files (*.json)", "All Files (*)"]
                                    : ["All Files (*)"]
                                onAccepted: {
                                    var file = fileDialog.selectedFile.toString()
                                    file = file.replace(/^(file:\/{2})/, "")
                                    if (root.graphModel) {
                                        var oldVal = root.graphModel.qmlNodeData(root.nodeId, modelData)
                                        if (root.undoManager)
                                            root.undoManager.qmlSetNodeData(root.nodeId, modelData, oldVal, file)
                                        else
                                            root.graphModel.qmlSetNodeData(root.nodeId, modelData, file)
                                    }
                                }
                            }

                            FolderDialog {
                                id: folderDialog
                                title: "Select folder"
                                onAccepted: {
                                    var folder = folderDialog.selectedFolder.toString()
                                    folder = folder.replace(/^(file:\/{2})/, "")
                                    if (root.graphModel) {
                                        var oldVal = root.graphModel.qmlNodeData(root.nodeId, modelData)
                                        if (root.undoManager)
                                            root.undoManager.qmlSetNodeData(root.nodeId, modelData, oldVal, folder)
                                        else
                                            root.graphModel.qmlSetNodeData(root.nodeId, modelData, folder)
                                    }
                                }
                            }
                        }
                    }

                    // Color swatch picker
                    Item {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        width: 64
                        height: 20
                        visible: inputDelegate.isColorPort && (root._connectionVersion, root.graphModel ? !root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true) : true)

                        Rectangle {
                            id: colorSwatch
                            anchors.fill: parent
                            radius: 4
                            border.color: "#555555"
                            border.width: 1

                            color: {
                                if (!root.graphModel) return "#FFFFFF"
                                root._dataVersion
                                var val = root.graphModel.qmlNodeData(root.nodeId, modelData)
                                if (val !== undefined && val !== null && val !== "" && !Array.isArray(val)) {
                                    var comps = root.colorToComponents(val)
                                    if (comps.x !== 0 || comps.y !== 0 || comps.z !== 0)
                                        return val
                                }
                                return "#FFFFFF"
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    var val = root.graphModel ? root.graphModel.qmlNodeData(root.nodeId, modelData) : undefined
                                    colorPickerPopup.openFor(root.nodeId, modelData, val)
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
                        visible: !inputDelegate.isFilePathPort && !inputDelegate.isColorPort && (root._connectionVersion, root.graphModel ? !root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true) : true)

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
                            visible: !inputField.activeFocus && (root._connectionVersion, root.graphModel ? !root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true) : true)
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

    // ── Color Picker Popup ──
    Popup {
        id: colorPickerPopup
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape

        property string editNodeId: ""
        property string editPort: ""
        property real pickerHue: 0
        property real pickerSat: 1.0
        property real pickerVal: 1.0
        property real pickerAlpha: 1.0
        property color currentColor: "#ffffff"

        width: 320
        height: 480
        x: root.width + 8
        y: Math.max(0, (root.height - height) / 2 - 40)

        function openFor(nodeId, portName, currentValue) {
            editNodeId = nodeId
            editPort = portName
            if (currentValue !== undefined && currentValue !== null && currentValue !== "" && !Array.isArray(currentValue)) {
                var c = root.colorToComponents(currentValue)
                if (c.x === 0 && c.y === 0 && c.z === 0 && c.w === 0)
                    currentColor = "#FFFFFF"
                else
                    currentColor = currentValue
            } else {
                currentColor = "#FFFFFF"
            }
            var hsva = root.colorToHsva(currentColor)
            pickerHue = hsva.x
            pickerSat = hsva.y
            pickerVal = hsva.z
            pickerAlpha = hsva.w
            open()
        }

        background: Rectangle {
            color: "#252525"
            radius: 10
            border.color: "#444"
            border.width: 1
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 12

            Text {
                text: "PICK COLOR"
                color: "#888"
                font.pixelSize: 10
                font.bold: true
                Layout.alignment: Qt.AlignLeft
            }

            // Side-by-side: Wheel and Alpha/Eyedropper
            RowLayout {
                Layout.fillWidth: true
                spacing: 15
                Layout.alignment: Qt.AlignTop

                // Left: Hue/Saturation Wheel
                Item {
                    id: wheelArea
                    Layout.preferredWidth: 220
                    Layout.preferredHeight: 220
                    Layout.alignment: Qt.AlignTop

                    Canvas {
                        id: wheelCanvas
                        anchors.fill: parent
                        antialiasing: true
                        property real wheelVal: colorPickerPopup.pickerVal

                        onWheelValChanged: requestPaint()

                        onPaint: {
                            var ctx = getContext("2d")
                            var w = width, h = height
                            var cx = w / 2, cy = h / 2
                            var r = Math.min(cx, cy) - 2
                            var val = colorPickerPopup.pickerVal

                            ctx.clearRect(0, 0, w, h)
                            
                            // Draw hue circle
                            for (var i = 0; i < 360; i += 1) {
                                var hue = i / 360
                                var a0 = i * Math.PI / 180 - Math.PI / 2 - 0.01
                                var a1 = (i + 1) * Math.PI / 180 - Math.PI / 2 + 0.01

                                ctx.beginPath()
                                ctx.moveTo(cx, cy)
                                ctx.arc(cx, cy, r, a0, a1)
                                ctx.fillStyle = Qt.hsva(hue, 1, val, 1)
                                ctx.fill()
                            }

                            // Draw saturation gradient (center is white)
                            var satGrad = ctx.createRadialGradient(cx, cy, 0, cx, cy, r)
                            satGrad.addColorStop(0, "white")
                            satGrad.addColorStop(1, "transparent")
                            ctx.fillStyle = satGrad
                            ctx.beginPath()
                            ctx.arc(cx, cy, r, 0, 2 * Math.PI)
                            ctx.fill()
                        }

                        MouseArea {
                            anchors.fill: parent
                            function pickColor(mx, my) {
                                var dx = mx - width / 2
                                var dy = my - height / 2
                                var dist = Math.sqrt(dx * dx + dy * dy)
                                var maxR = width / 2 - 2
                                
                                var angle = Math.atan2(dy, dx) + Math.PI / 2
                                if (angle < 0) angle += 2 * Math.PI
                                
                                colorPickerPopup.pickerHue = angle / (2 * Math.PI)
                                colorPickerPopup.pickerSat = Math.min(1.0, dist / maxR)
                            }
                            onPressed: pickColor(mouseX, mouseY)
                            onPositionChanged: pickColor(mouseX, mouseY)
                        }
                    }

                    // Indicator for H/S
                    Rectangle {
                        property real angle: colorPickerPopup.pickerHue * 2 * Math.PI - Math.PI / 2
                        property real pickDist: colorPickerPopup.pickerSat * (parent.width / 2 - 2)
                        x: parent.width / 2 + Math.cos(angle) * pickDist - 8
                        y: parent.height / 2 + Math.sin(angle) * pickDist - 8
                        width: 16
                        height: 16
                        radius: 8
                        color: "transparent"
                        border.color: "white"
                        border.width: 2
                        
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 2
                            radius: 6
                            color: "transparent"
                            border.color: "black"
                            border.width: 1
                        }
                    }
                }

                // Right: Alpha Slider and Eyedropper
                ColumnLayout {
                    Layout.fillHeight: true
                    spacing: 12
                    Layout.alignment: Qt.AlignTop

                    // Alpha Slider (Vertical)
                    Item {
                        id: alphaBar
                        Layout.preferredWidth: 32
                        Layout.fillHeight: true
                        Layout.minimumHeight: 176

                        Rectangle {
                            anchors.fill: parent
                            radius: 4
                            clip: true
                            border.color: "#444"
                            
                            // Checkerboard
                            Grid {
                                anchors.fill: parent
                                columns: 2
                                Repeater {
                                    model: 22
                                    Rectangle { width: 16; height: 16; color: index % 2 === (Math.floor(index/2)%2) ? "#333" : "#444" }
                                }
                            }

                            Rectangle {
                                anchors.fill: parent
                                radius: 4
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, 1.0) }
                                    GradientStop { position: 1.0; color: "transparent" }
                                }
                            }

                            // Handle
                            Rectangle {
                                width: parent.width + 4
                                height: 8
                                x: -2
                                y: (1.0 - colorPickerPopup.pickerAlpha) * (parent.height - 8)
                                color: "white"
                                radius: 2
                                border.color: "black"
                            }

                            MouseArea {
                                anchors.fill: parent
                                function updateAlpha(my) {
                                    colorPickerPopup.pickerAlpha = 1.0 - Math.max(0, Math.min(1, my / height))
                                }
                                onPressed: updateAlpha(mouseY)
                                onPositionChanged: updateAlpha(mouseY)
                            }
                        }
                    }

                    // Eye Drop Button (Small square)
                    Button {
                        id: eyeDropperBtn
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 32
                        padding: 0
                        
                        background: Rectangle {
                            color: eyeDropperBtn.hovered ? "#444" : "#333"
                            radius: 4
                            border.color: "#555"
                            
                            Text {
                                anchors.centerIn: parent
                                text: "\u2316"
                                color: "white"
                                font.pixelSize: 18
                            }
                        }

                        onClicked: {
                            colorPickerPopup.close()
                            Qt.callLater(function() {
                                var overlayParent = Overlay.overlay
                                if (!overlayParent) return
                                var overlay = eyedropperOverlayComp.createObject(overlayParent, {
                                    onPicked: function(screenX, screenY) {
                                        if (root.graphModel) {
                                            var hexStr = root.graphModel.qmlScreenColorAt(screenX, screenY)
                                            if (hexStr) {
                                                var hsva = root.colorToHsva(hexStr)
                                                colorPickerPopup.pickerHue = hsva.x
                                                colorPickerPopup.pickerSat = hsva.y
                                                colorPickerPopup.pickerVal = hsva.z
                                                colorPickerPopup.pickerAlpha = hsva.w
                                            }
                                        }
                                        colorPickerPopup.open()
                                    },
                                    onCancelled: function() {
                                        colorPickerPopup.open()
                                    }
                                })
                            })
                        }
                    }
                }
            }

            // Value / Brightness Slider (Horizontal)
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 18
                    radius: 4
                    border.color: "#444"
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: "black" }
                        GradientStop { position: 1.0; color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, 1.0, 1.0) }
                    }

                    Rectangle {
                        width: 10
                        height: parent.height + 4
                        anchors.verticalCenter: parent.verticalCenter
                        x: colorPickerPopup.pickerVal * (parent.width - 10)
                        color: "white"
                        radius: 2
                        border.color: "black"
                    }

                    MouseArea {
                        anchors.fill: parent
                        function updateVal(mx) {
                            colorPickerPopup.pickerVal = Math.max(0, Math.min(1, mx / width))
                        }
                        onPressed: updateVal(mouseX)
                        onPositionChanged: updateVal(mouseX)
                    }
                }
                
                Text {
                    text: "BRIGHTNESS"
                    color: "#666"
                    font.pixelSize: 9
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            // Hex Display and Preview
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    color: "#151515"
                    radius: 4
                    border.color: "#333"
                    
                    TextInput {
                        anchors.centerIn: parent
                        text: {
                            var c = Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, colorPickerPopup.pickerAlpha)
                            return String(c).toUpperCase()
                        }
                        color: "white"
                        font.family: "monospace"
                        font.pixelSize: 14
                        readOnly: true
                        selectByMouse: true
                    }
                }

                // Color Preview with Checkerboard
                Rectangle {
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 36
                    radius: 4
                    border.color: "#333"
                    clip: true
                    
                    Grid {
                        anchors.fill: parent
                        columns: 5
                        Repeater {
                            model: 15
                            Rectangle { width: 16; height: 18; color: (Math.floor(index/5) + index) % 2 === 0 ? "#333" : "#444" }
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, colorPickerPopup.pickerAlpha)
                    }
                }
            }

            // Bottom Labels (matching SVG label placement)
            Text {
                text: "HUE AND SATURATION"
                color: "#666"
                font.pixelSize: 9
                font.bold: true
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: 40 // Align roughly with wheel
            }

            // Buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                Layout.topMargin: 5

                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: colorPickerPopup.close()
                    background: Rectangle {
                        color: parent.down ? "#222" : (parent.hovered ? "#333" : "#2A2A2A")
                        radius: 4
                        border.color: "#444"
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                    }
                }

                Button {
                    text: "OK"
                    Layout.fillWidth: true
                    highlighted: true
                    onClicked: {
                        if (root.graphModel) {
                            var oldVal = root.graphModel.qmlNodeData(colorPickerPopup.editNodeId, colorPickerPopup.editPort)
                            var newColor = Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, colorPickerPopup.pickerAlpha)
                            if (root.undoManager)
                                root.undoManager.qmlSetNodeData(colorPickerPopup.editNodeId, colorPickerPopup.editPort, oldVal, newColor)
                            else
                                root.graphModel.qmlSetNodeData(colorPickerPopup.editNodeId, colorPickerPopup.editPort, newColor)
                        }
                        colorPickerPopup.close()
                    }
                    background: Rectangle {
                        color: parent.down ? "#006699" : (parent.hovered ? "#0099DD" : "#0077BB")
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.bold: true
                    }
                }
            }
        }
    }

    Component {
        id: eyedropperOverlayComp

        Rectangle {
            id: eyeOverlay
            anchors.fill: parent
            color: "transparent"
            property var onPicked: null
            property var onCancelled: null

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.CrossCursor
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked: function(mouse) {
                    if (mouse.button === Qt.RightButton) {
                        if (eyeOverlay.onCancelled)
                            eyeOverlay.onCancelled()
                        eyeOverlay.destroy()
                        return
                    }
                    var globalPt = mapToGlobal(mouse.x, mouse.y)
                    if (eyeOverlay.onPicked)
                        eyeOverlay.onPicked(globalPt.x, globalPt.y)
                    eyeOverlay.destroy()
                }
            }

            Keys.onEscapePressed: {
                if (eyeOverlay.onCancelled)
                    eyeOverlay.onCancelled()
                eyeOverlay.destroy()
            }

            focus: true
            Component.onCompleted: forceActiveFocus()
        }
    }
}
