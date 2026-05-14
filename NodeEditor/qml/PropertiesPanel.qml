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
                            visible: propDelegate.isColorPort

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
                                visible: !propDelegate.isFilePathPort

                                text: {
                                    if (!root.graphModel) return ""
                                    root._dataVersion
                                    var val = root.graphModel.qmlNodeData(root.nodeId, modelData)
                                    return val !== undefined && val !== null ? String(val) : ""
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
                                visible: !propInputField.activeFocus && !propDelegate.isFilePathPort
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

        width: 260
        height: 420
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
            color: "#2A2A2A"
            radius: 8
            border.color: "#555555"
            border.width: 1
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8

            Item {
                Layout.preferredWidth: 220
                Layout.preferredHeight: 220
                Layout.alignment: Qt.AlignHCenter

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
                        var r = Math.min(cx, cy) - 1
                        var val = propColorPickerPopup.pickerVal

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
                            var cx = mx / width, cy = my / height
                            var dx = cx - 0.5, dy = cy - 0.5
                            var dist = Math.sqrt(dx * dx + dy * dy) * 2.0
                            if (dist > 1.0) return
                            var angle = Math.atan2(dy, dx) / (2 * Math.PI) + 0.5
                            if (angle < 0) angle += 1.0
                            propColorPickerPopup.pickerHue = angle
                            propColorPickerPopup.pickerSat = dist
                        }
                    }
                }

                Rectangle {
                    x: parent.width / 2 + Math.cos(2 * Math.PI * propColorPickerPopup.pickerHue - Math.PI) * (parent.width / 2 - 10) - 7
                    y: parent.height / 2 + Math.sin(2 * Math.PI * propColorPickerPopup.pickerHue - Math.PI) * (parent.height / 2 - 10) - 7
                    width: 14; height: 14; radius: 7
                    border.color: "#FFFFFF"; border.width: 2
                    color: "transparent"
                }

                Rectangle {
                    x: parent.width / 2 + Math.cos(2 * Math.PI * propColorPickerPopup.pickerHue - Math.PI) * (parent.width / 2) * propColorPickerPopup.pickerSat - 4
                    y: parent.height / 2 + Math.sin(2 * Math.PI * propColorPickerPopup.pickerHue - Math.PI) * (parent.height / 2) * propColorPickerPopup.pickerSat - 4
                    width: 8; height: 8; radius: 4
                    border.color: "#FFFFFF"; border.width: 1.5
                    color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, 1.0)
                }
            }

            RowLayout {
                Layout.fillWidth: true; spacing: 6
                Text { text: "V"; color: "#CCCCCC"; font.pixelSize: 11; font.bold: true; Layout.alignment: Qt.AlignVCenter }

                Rectangle {
                    Layout.fillWidth: true; Layout.preferredHeight: 16; radius: 3; clip: true

                    Rectangle {
                        anchors.fill: parent; radius: 3
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, 0.0, 1.0) }
                            GradientStop { position: 1.0; color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, 1.0, 1.0) }
                        }
                    }

                    Rectangle {
                        x: (parent.width - 6) * propColorPickerPopup.pickerVal
                        y: 0; width: 6; height: parent.height; color: "white"; radius: 1
                    }

                    MouseArea {
                        anchors.fill: parent
                        onPressed: updateVal(mouseX)
                        onPositionChanged: updateVal(mouseX)
                        function updateVal(mx) {
                            propColorPickerPopup.pickerVal = Math.max(0, Math.min(1, mx / width))
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
                            GradientStop { position: 0.0; color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, 0.0) }
                            GradientStop { position: 1.0; color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, 1.0) }
                        }
                    }

                    Rectangle {
                        x: (parent.width - 6) * propColorPickerPopup.pickerAlpha
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
                            propColorPickerPopup.pickerAlpha = Math.max(0, Math.min(1, mx / width))
                        }
                    }
                }
            }

            // Eyedropper + Color preview row
            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Button {
                    id: propEyeDropperBtn
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
                        color: Qt.hsva(propColorPickerPopup.pickerHue, propColorPickerPopup.pickerSat, propColorPickerPopup.pickerVal, propColorPickerPopup.pickerAlpha)
                    }
                }
            }

            // Buttons
            RowLayout {
                Layout.alignment: Qt.AlignRight; spacing: 8

                Button { text: "Cancel"; flat: true; onClicked: propColorPickerPopup.close() }

                Button {
                    text: "OK"; highlighted: true
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
