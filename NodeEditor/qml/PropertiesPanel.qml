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
    readonly property var _emptyList: []

    property bool _isDisplayNode: false
    property int _dataVersion: 0
    property int _connectionVersion: 0

    function truncate(val) {
        if (val === undefined || val === null) return ""
        var s = String(val)
        if (s.length > 5) return s.substring(0, 5) + ".."
        return s
    }

    onNodeIdChanged: {
        if (nodeId && graphModel)
            nodeInfo = graphModel.qmlNodeInfo(nodeId)
        else
            nodeInfo = ({})
        _isDisplayNode = !!(nodeInfo && nodeInfo.type && (
            nodeInfo.type.indexOf("output/display/") === 0 ||
            nodeInfo.type.indexOf("output/visual/") === 0))
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

    Connections {
        target: graphModel
        function onQmlNodeDataChanged(id, key) {
            if (id === root.nodeId) {
                root.nodeInfo = graphModel.qmlNodeInfo(root.nodeId)
                root._dataVersion++
            }
        }
        function onQmlNodePositionChanged(id) {
            if (id === root.nodeId)
                root.nodeInfo = graphModel.qmlNodeInfo(root.nodeId)
        }
        function onQmlEdgeAdded(id) {
            if (!root.graphModel) return
            var info = root.graphModel.qmlEdgeInfo(id)
            if (info && (info.targetNodeId === root.nodeId || info.sourceNodeId === root.nodeId))
                root._connectionVersion++
        }
        function onQmlEdgeRemoved(id) {
            root._connectionVersion++
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 4

        Label {
            text: "Properties"
            color: "#CCCCCC"
            font.pixelSize: 14
            font.bold: true
            padding: 10
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#3A3A3A"
        }

        Label {
            text: root.nodeId ? root.nodeInfo.type || "Node" : "Select a node"
            color: root.nodeId ? "#FFFFFF" : "#777777"
            font.pixelSize: 13
            font.bold: root.nodeId !== ""
            padding: 10
            Layout.fillWidth: true
        }

        Rectangle {
            visible: root.nodeId !== ""
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            color: "#2A2A2A"
            radius: 4
            implicitHeight: detailColumn.height + 20

            ColumnLayout {
                id: detailColumn
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 10
                spacing: 6

                Label {
                    text: "ID: " + (root.nodeId ? root.nodeId.substring(0, 8) + "..." : "")
                    color: "#999999"
                    font.pixelSize: 10
                }

                Label {
                    text: "Type: " + (root.nodeInfo.type || "")
                    color: "#CCCCCC"
                    font.pixelSize: 11
                }

                Label {
                    text: "Position: (" + Math.round((root.nodeInfo.x || 0)) + ", " + Math.round((root.nodeInfo.y || 0)) + ")"
                    color: "#CCCCCC"
                    font.pixelSize: 11
                }

                // Input port data
                Repeater {
                    model: root.nodeInfo.inputPorts || root._emptyList

                    Item {
                        id: propDelegate
                        Layout.fillWidth: true
                        height: isColorPort ? 26 : 24
                        property string dataOldVal: ""

                        readonly property bool isColorPort: root.nodeInfo.inputPortTypes && index < root.nodeInfo.inputPortTypes.length && root.nodeInfo.inputPortTypes[index] === 4
                        readonly property bool isFilePathPort: modelData === "filePath" || modelData === "path" || modelData === "folderPath"
                        readonly property bool isFileWritePort: isFilePathPort && root.nodeInfo.type && (
                            root.nodeInfo.type.indexOf("/write") >= 0 || root.nodeInfo.type.indexOf("/fileWriter") >= 0
                        )
                        readonly property bool isFolderSelectorPort: isFilePathPort && root.nodeInfo.type === "system/file/folderSelector"

                        Label {
                            text: modelData + ":"
                            color: "#AAAAAA"
                            font.pixelSize: 11
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.right: inputContainer.left
                            anchors.rightMargin: 4
                            elide: Text.ElideRight
                        }

                        // Color swatch picker
                        Item {
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            width: 64
                            height: 20
                            visible: propDelegate.isColorPort && (root._connectionVersion, root.graphModel ? !root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true) : true)

                            Rectangle {
                                id: propColorSwatch
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
                                        propColorPickerPopup.openFor(root.nodeId, modelData, val)
                                    }
                                }
                            }
                        }

                        Item {
                            id: inputContainer
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            width: isFilePathPort ? 100 : 80
                            height: 22
                            visible: !propDelegate.isColorPort

                            // File path browse row
                            Row {
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 4
                                visible: propDelegate.isFilePathPort

                                Text {
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
                                    color: parent.children[0].text === "No file" ? "#666666" : "#CCCCCC"
                                    font.pixelSize: 10
                                    elide: Text.ElideLeft
                                    width: 70
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Button {
                                    width: 24
                                    height: 20
                                    padding: 0
                                    text: "..."
                                    font.pixelSize: 10
                                    onClicked: {
                                        if (propDelegate.isFolderSelectorPort)
                                            propFolderDialog.open()
                                        else
                                            propFileDialog.open()
                                    }

                                    FileDialog {
                                        id: propFileDialog
                                        title: propDelegate.isFileWritePort ? "Select output file" : "Select file"
                                        fileMode: propDelegate.isFileWritePort ? FileDialog.SaveFile : FileDialog.OpenFile
                                        nameFilters: modelData === "filePath"
                                            ? ["JSON Files (*.json)", "All Files (*)"]
                                            : ["All Files (*)"]
                                        onAccepted: {
                                            var file = propFileDialog.selectedFile.toString()
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
                                        id: propFolderDialog
                                        title: "Select folder"
                                        onAccepted: {
                                            var folder = propFolderDialog.selectedFolder.toString()
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

                            TextField {
                                id: propInputField
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
                                visible: !propDelegate.isFilePathPort && (root._connectionVersion, root.graphModel ? !root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true) : true)

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
                                        propDelegate.dataOldVal = text
                                }

                                onEditingFinished: {
                                    if (root.undoManager && propDelegate.dataOldVal !== text)
                                        root.undoManager.qmlSetNodeData(root.nodeId, modelData, propDelegate.dataOldVal, text)
                                }
                            }

                            Text {
                                anchors.fill: parent
                                anchors.rightMargin: 4
                                horizontalAlignment: Text.AlignRight
                                verticalAlignment: Text.AlignVCenter
                                color: "#FFFFFF"
                                font.pixelSize: 10
                                text: root.truncate(propInputField.text)
                                visible: !propInputField.activeFocus && !propDelegate.isFilePathPort && (root._connectionVersion, root.graphModel ? !root.graphModel.qmlIsPortConnected(root.nodeId, modelData, true) : true)
                                clip: true
                            }
                        }
                    }
                }

                // Output port data
                Repeater {
                    model: root.nodeInfo.outputPorts || root._emptyList
                    visible: !root._isDisplayNode

                    Item {
                        Layout.fillWidth: true
                        height: 20

                        Label {
                            text: modelData + " (out):"
                            color: "#888888"
                            font.pixelSize: 11
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.right: outValLabel.left
                            anchors.rightMargin: 4
                            elide: Text.ElideRight
                        }

                        Label {
                            id: outValLabel
                            text: {
                                if (!root.graphModel || !root.nodeId) return ""
                                var val = root.graphModel.qmlNodeData(root.nodeId, modelData)
                                return root.truncate(val)
                            }
                            color: "#AAAAAA"
                            font.pixelSize: 11
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                        }
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }

    // ── Color Picker Popup ──
    Popup {
        id: propColorPickerPopup
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape

        property string editNodeId: ""
        property string editPort: ""
        property real pickerHue: 0
        property real pickerSat: 1.0
        property real pickerVal: 1.0
        property real pickerAlpha: 1.0

        width: 320
        height: 480
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        function openFor(nodeId, portName, currentValue) {
            editNodeId = nodeId
            editPort = portName
            var color
            if (currentValue !== undefined && currentValue !== null && currentValue !== "" && !Array.isArray(currentValue)) {
                var c = root.colorToComponents(currentValue)
                if (c.x === 0 && c.y === 0 && c.z === 0 && c.w === 0)
                    color = "#FFFFFF"
                else
                    color = currentValue
            } else {
                color = "#FFFFFF"
            }
            var hsva = root.colorToHsva(color)
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
                    id: propWheelArea
                    Layout.preferredWidth: 220
                    Layout.preferredHeight: 220
                    Layout.alignment: Qt.AlignTop

                    Canvas {
                        id: propWheelCanvas
                        anchors.fill: parent
                        antialiasing: true
                        property real wheelVal: propColorPickerPopup.pickerVal

                        onWheelValChanged: requestPaint()

                        onPaint: {
                            var ctx = getContext("2d")
                            var w = width, h = height
                            var cx = w / 2, cy = h / 2
                            var r = Math.min(cx, cy) - 2
                            var val = propColorPickerPopup.pickerVal

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
                                
                                propColorPickerPopup.pickerHue = angle / (2 * Math.PI)
                                propColorPickerPopup.pickerSat = Math.min(1.0, dist / maxR)
                            }
                            onPressed: pickColor(mouseX, mouseY)
                            onPositionChanged: pickColor(mouseX, mouseY)
                        }
                    }

                    // Indicator for H/S
                    Rectangle {
                        property real angle: propColorPickerPopup.pickerHue * 2 * Math.PI - Math.PI / 2
                        property real pickDist: propColorPickerPopup.pickerSat * (parent.width / 2 - 2)
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
                        id: propAlphaBar
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
                                    GradientStop { position: 0.0; color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, 1.0) }
                                    GradientStop { position: 1.0; color: "transparent" }
                                }
                            }

                            // Handle
                            Rectangle {
                                width: parent.width + 4
                                height: 8
                                x: -2
                                y: (1.0 - propColorPickerPopup.pickerAlpha) * (parent.height - 8)
                                color: "white"
                                radius: 2
                                border.color: "black"
                            }

                            MouseArea {
                                anchors.fill: parent
                                function updateAlpha(my) {
                                    propColorPickerPopup.pickerAlpha = 1.0 - Math.max(0, Math.min(1, my / height))
                                }
                                onPressed: updateAlpha(mouseY)
                                onPositionChanged: updateAlpha(mouseY)
                            }
                        }
                    }

                    // Eye Drop Button (Small square)
                    Button {
                        id: propEyeDropperBtn
                        Layout.preferredWidth: 32
                        Layout.preferredHeight: 32
                        padding: 0
                        
                        background: Rectangle {
                            color: propEyeDropperBtn.hovered ? "#444" : "#333"
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
                            propColorPickerPopup.close()
                            Qt.callLater(function() {
                                var overlayParent = Overlay.overlay
                                if (!overlayParent) return
                                var overlay = propEyedropperOverlayComp.createObject(overlayParent, {
                                    onPicked: function(screenX, screenY) {
                                        if (root.graphModel) {
                                            var hexStr = root.graphModel.qmlScreenColorAt(screenX, screenY)
                                            if (hexStr) {
                                                var hsva = root.colorToHsva(hexStr)
                                                propColorPickerPopup.pickerHue = hsva.x
                                                propColorPickerPopup.pickerSat = hsva.y
                                                propColorPickerPopup.pickerVal = hsva.z
                                                propColorPickerPopup.pickerAlpha = hsva.w
                                            }
                                        }
                                        propColorPickerPopup.open()
                                    },
                                    onCancelled: function() {
                                        propColorPickerPopup.open()
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
                        GradientStop { position: 1.0; color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, 1.0, 1.0) }
                    }

                    Rectangle {
                        width: 10
                        height: parent.height + 4
                        anchors.verticalCenter: parent.verticalCenter
                        x: propColorPickerPopup.pickerVal * (parent.width - 10)
                        color: "white"
                        radius: 2
                        border.color: "black"
                    }

                    MouseArea {
                        anchors.fill: parent
                        function updateVal(mx) {
                            propColorPickerPopup.pickerVal = Math.max(0, Math.min(1, mx / width))
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
                            var c = Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, propColorPickerPopup.pickerAlpha)
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
                        color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, propColorPickerPopup.pickerAlpha)
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
                    onClicked: propColorPickerPopup.close()
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
                            var oldVal = root.graphModel.qmlNodeData(propColorPickerPopup.editNodeId, propColorPickerPopup.editPort)
                            var newColor = Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, propColorPickerPopup.pickerAlpha)
                            if (root.undoManager)
                                root.undoManager.qmlSetNodeData(propColorPickerPopup.editNodeId, propColorPickerPopup.editPort, oldVal, newColor)
                            else
                                root.graphModel.qmlSetNodeData(propColorPickerPopup.editNodeId, propColorPickerPopup.editPort, newColor)
                        }
                        propColorPickerPopup.close()
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
        id: propEyedropperOverlayComp

        Rectangle {
            id: propEyeOverlay
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
                        if (propEyeOverlay.onCancelled)
                            propEyeOverlay.onCancelled()
                        propEyeOverlay.destroy()
                        return
                    }
                    var globalPt = mapToGlobal(mouse.x, mouse.y)
                    if (propEyeOverlay.onPicked)
                        propEyeOverlay.onPicked(globalPt.x, globalPt.y)
                    propEyeOverlay.destroy()
                }
            }

            Keys.onEscapePressed: {
                if (propEyeOverlay.onCancelled)
                    propEyeOverlay.onCancelled()
                propEyeOverlay.destroy()
            }

            focus: true
            Component.onCompleted: forceActiveFocus()
        }
    }
}
