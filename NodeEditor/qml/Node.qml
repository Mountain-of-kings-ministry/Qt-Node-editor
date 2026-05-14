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
                        visible: inputDelegate.isColorPort

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
                        visible: !inputDelegate.isFilePathPort && !inputDelegate.isColorPort

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

        width: 260
        height: 420
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
            color: "#2A2A2A"
            radius: 8
            border.color: "#555555"
            border.width: 1
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            // Color wheel
            Item {
                Layout.preferredWidth: 220
                Layout.preferredHeight: 220
                Layout.alignment: Qt.AlignHCenter

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
                        var r = Math.min(cx, cy) - 1
                        var val = colorPickerPopup.pickerVal

                        ctx.clearRect(0, 0, w, h)
                        ctx.save()
                        ctx.beginPath()
                        ctx.arc(cx, cy, r, 0, 2 * Math.PI)
                        ctx.clip()

                        var steps = 360
                        for (var i = 0; i < steps; i++) {
                            var hue = i / steps
                            var a0 = 2 * Math.PI * i / steps - Math.PI / 2
                            var a1 = 2 * Math.PI * (i + 1) / steps - Math.PI / 2

                            ctx.beginPath()
                            ctx.moveTo(cx, cy)
                            ctx.arc(cx, cy, r, a0, a1)
                            ctx.closePath()
                            ctx.fillStyle = Qt.hsva(hue, 1, val, 1)
                            ctx.fill()
                        }

                        var satGrad = ctx.createRadialGradient(cx, cy, 0, cx, cy, r)
                        satGrad.addColorStop(0, Qt.rgba(1, 1, 1, 1))
                        satGrad.addColorStop(1, Qt.rgba(1, 1, 1, 0))
                        ctx.fillStyle = satGrad
                        ctx.fillRect(0, 0, w, h)

                        ctx.restore()
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressed: pickColor(mouseX, mouseY)
                        onPositionChanged: pickColor(mouseX, mouseY)

                        function pickColor(mx, my) {
                            var cx = mx / width
                            var cy = my / height
                            var dx = cx - 0.5
                            var dy = cy - 0.5
                            var dist = Math.sqrt(dx * dx + dy * dy) * 2.0
                            if (dist > 1.0) return
                            var angle = Math.atan2(dy, dx) / (2 * Math.PI) + 0.5
                            if (angle < 0) angle += 1.0
                            colorPickerPopup.pickerHue = angle
                            colorPickerPopup.pickerSat = dist
                        }
                    }
                }

                // Hue indicator ring
                Rectangle {
                    x: parent.width / 2 + Math.cos(2 * Math.PI * colorPickerPopup.pickerHue - Math.PI) * (parent.width / 2 - 10) - 7
                    y: parent.height / 2 + Math.sin(2 * Math.PI * colorPickerPopup.pickerHue - Math.PI) * (parent.height / 2 - 10) - 7
                    width: 14
                    height: 14
                    radius: 7
                    border.color: "#FFFFFF"
                    border.width: 2
                    color: "transparent"
                }

                // Saturation indicator dot
                Rectangle {
                    x: parent.width / 2 + Math.cos(2 * Math.PI * colorPickerPopup.pickerHue - Math.PI) * (parent.width / 2) * colorPickerPopup.pickerSat - 4
                    y: parent.height / 2 + Math.sin(2 * Math.PI * colorPickerPopup.pickerHue - Math.PI) * (parent.height / 2) * colorPickerPopup.pickerSat - 4
                    width: 8
                    height: 8
                    radius: 4
                    border.color: "#FFFFFF"
                    border.width: 1.5
                    color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, 1.0)
                }
            }

            // Value slider
            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Text {
                    text: "V"
                    color: "#CCCCCC"
                    font.pixelSize: 11
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 16
                    radius: 3
                    clip: true

                    Rectangle {
                        anchors.fill: parent
                        radius: 3
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, 0.0, 1.0) }
                            GradientStop { position: 1.0; color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, 1.0, 1.0) }
                        }
                    }

                    Rectangle {
                        x: (parent.width - 6) * colorPickerPopup.pickerVal
                        y: 0
                        width: 6
                        height: parent.height
                        color: "white"
                        radius: 1
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressed: updateVal(mouseX)
                        onPositionChanged: updateVal(mouseX)
                        function updateVal(mx) {
                            colorPickerPopup.pickerVal = Math.max(0, Math.min(1, mx / width))
                        }
                    }
                }
            }

            // Opacity slider
            RowLayout {
                Layout.fillWidth: true
                spacing: 6

                Text {
                    text: "A"
                    color: "#CCCCCC"
                    font.pixelSize: 11
                    font.bold: true
                    Layout.alignment: Qt.AlignVCenter
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 16
                    radius: 3
                    clip: true

                    Rectangle {
                        anchors.fill: parent
                        radius: 3
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, 0.0) }
                            GradientStop { position: 1.0; color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, 1.0) }
                        }
                    }

                    Rectangle {
                        x: (parent.width - 6) * colorPickerPopup.pickerAlpha
                        y: 0
                        width: 6
                        height: parent.height
                        color: "white"
                        radius: 1
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressed: updateA(mouseX)
                        onPositionChanged: updateA(mouseX)
                        function updateA(mx) {
                            colorPickerPopup.pickerAlpha = Math.max(0, Math.min(1, mx / width))
                        }
                    }
                }
            }

            // Eyedropper + Color preview row
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Button {
                    id: eyeDropperBtn
                    text: "\uD83D\uDD76"
                    flat: true
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    font.pixelSize: 16
                    ToolTip {
                        text: "Pick color from screen"
                        visible: parent.hovered
                        delay: 500
                    }
                    background: Rectangle {
                        color: parent.hovered ? "#3A3A3A" : "#2A2A2A"
                        radius: 4
                        border.color: "#555555"
                        border.width: 1
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

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    radius: 4
                    border.color: "#555555"
                    border.width: 1
                    clip: true

                    // Checkerboard background for alpha
                    Row {
                        anchors.fill: parent
                        Repeater {
                            model: 8
                            Rectangle {
                                width: parent.width / 8
                                height: parent.height
                                color: (index % 2 === 0) ? "#CCCCCC" : "#FFFFFF"
                            }
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Qt.hsva(colorPickerPopup.pickerHue, colorPickerPopup.pickerSat, colorPickerPopup.pickerVal, colorPickerPopup.pickerAlpha)
                    }
                }
            }

            // Buttons
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 8

                Button {
                    text: "Cancel"
                    flat: true
                    onClicked: colorPickerPopup.close()
                }

                Button {
                    text: "OK"
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
