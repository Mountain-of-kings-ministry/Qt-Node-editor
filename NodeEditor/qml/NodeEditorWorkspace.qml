import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import NodeEditor

Item {
    id: root

    property var graphModel: null
    property var undoManager: null

    property string selectedNodeId: ""

    property var _appUndoManager: typeof _undoManager !== "undefined" ? _undoManager : null
    property var _appGraphModel: typeof _graphModel !== "undefined" ? _graphModel : null

    property var effectiveGraphModel: graphModel || _appGraphModel
    property var effectiveUndoManager: undoManager || _appUndoManager

    property real defaultZoom: 1.0

    property alias canvas: canvasItem

    signal nodeSelected(string nodeId)
    signal nodeDeselected()

    // ── Top Bar ────────────────────────────────────────────────
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            height: 36
            color: "#2A2A2A"
            z: 10

            RowLayout {
                anchors.left: parent.left
                anchors.leftMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2

                // File
                ToolButton {
                    text: "File"
                    flat: true
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: "#CCCCCC"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }

                    onClicked: fileMenu.open()

                    Menu {
                        id: fileMenu
                        y: parent.height

                        MenuItem {
                            text: "New"
                            onTriggered: {
                                effectiveGraphModel.clear()
                                if (effectiveUndoManager) effectiveUndoManager.clear()
                            }
                        }
                        MenuItem {
                            text: "Open..."
                            onTriggered: openFileDialog.open()
                        }
                        MenuItem {
                            text: "Save"
                            onTriggered: saveCurrentGraph()
                        }
                        MenuItem {
                            text: "Save As..."
                            onTriggered: saveAsDialog.open()
                        }
                        MenuSeparator {}
                        MenuItem {
                            text: "Clear All"
                            onTriggered: {
                                effectiveGraphModel.clear()
                                if (effectiveUndoManager) effectiveUndoManager.clear()
                            }
                        }
                    }
                }

                // Separator
                Rectangle { width: 1; height: 20; color: "#444444"; Layout.alignment: Qt.AlignVCenter }

                // Add
                ToolButton {
                    text: "Add"
                    flat: true
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: "#CCCCCC"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }
                    onClicked: addNodePopup.open()
                }

                // Fit
                ToolButton {
                    text: "Fit"
                    flat: true
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: "#CCCCCC"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }
                    onClicked: canvasItem.fitToView()
                }

                // Zoom
                RowLayout {
                    spacing: 2
                    ToolButton {
                        text: "-"
                        flat: true
                        font.pixelSize: 14
                        contentItem: Text { text: parent.text; color: "#CCCCCC"; font.pixelSize: 14; horizontalAlignment: Text.AlignHCenter }
                        background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3; width: 24; height: 24 }
                        onClicked: canvasItem.zoom = Math.max(0.1, canvasItem.zoom / 1.15)
                    }
                    Label {
                        text: Math.round(canvasItem.zoom * 100) + "%"
                        color: "#CCCCCC"
                        font.pixelSize: 10
                        horizontalAlignment: Text.AlignHCenter
                        width: 40
                    }
                    ToolButton {
                        text: "+"
                        flat: true
                        font.pixelSize: 14
                        contentItem: Text { text: parent.text; color: "#CCCCCC"; font.pixelSize: 14; horizontalAlignment: Text.AlignHCenter }
                        background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3; width: 24; height: 24 }
                        onClicked: canvasItem.zoom = Math.min(5, canvasItem.zoom * 1.15)
                    }
                }

                Rectangle { width: 1; height: 20; color: "#444444"; Layout.alignment: Qt.AlignVCenter }

                // Select mode toggle
                ToolButton {
                    id: selectModeBtn
                    text: "Select"
                    flat: true
                    checkable: true
                    checked: canvasItem.selectMode === "box"
                    font.pixelSize: 11
                    contentItem: Text {
                        text: parent.text + (canvasItem.selectMode === "box" ? " [Box]" : "")
                        color: parent.checked ? "#FFFFFF" : "#CCCCCC"
                        font.pixelSize: 11
                    }
                    background: Rectangle {
                        color: parent.checked ? "#4A9EFF" : parent.hovered ? "#3A3A3A" : "transparent"
                        radius: 3
                    }
                    onClicked: {
                        canvasItem.selectMode = canvasItem.selectMode === "box" ? "default" : "box"
                    }
                }

                Rectangle { width: 1; height: 20; color: "#444444"; Layout.alignment: Qt.AlignVCenter }

                // Undo / Redo
                ToolButton {
                    text: "Undo"
                    flat: true
                    enabled: effectiveUndoManager ? effectiveUndoManager.canUndo : false
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: parent.enabled ? "#CCCCCC" : "#555555"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }
                    onClicked: effectiveUndoManager.undo()
                }
                ToolButton {
                    text: "Redo"
                    flat: true
                    enabled: effectiveUndoManager ? effectiveUndoManager.canRedo : false
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: parent.enabled ? "#CCCCCC" : "#555555"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }
                    onClicked: effectiveUndoManager.redo()
                }

                Item { Layout.fillWidth: true }

                // Compute / Auto
                ToolButton {
                    text: "Compute"
                    flat: true
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: "#CCCCCC"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }
                    onClicked: computeGraph()
                }
                ToolButton {
                    id: autoBtn
                    text: "Auto"
                    flat: true
                    checkable: true
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: parent.checked ? "#4CDF8B" : "#CCCCCC"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }
                }
            }
        }

        // ── Main Content ────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1E1E1E"

            NodeCanvas {
                id: canvasItem
                anchors.fill: parent
                graphModel: effectiveGraphModel
                undoManager: effectiveUndoManager

                onNodeSelected: function(nodeId) {
                    root.selectedNodeId = nodeId
                    root.nodeSelected(nodeId)
                    propertiesPanel.nodeId = nodeId
                }

                onNodeDeselected: {
                    root.selectedNodeId = ""
                    root.nodeDeselected()
                    propertiesPanel.nodeId = ""
                }
            }
        }

        // ── Bottom Bar ──────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            height: 28
            color: "#252525"
            z: 10

            RowLayout {
                anchors.left: parent.left
                anchors.leftMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2

                Repeater {
                    model: tabModel

                    delegate: Rectangle {
                        height: 22
                        width: 120
                        radius: 3
                        color: active ? "#3A3A3A" : "#2A2A2A"
                        border.color: active ? "#555555" : "transparent"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 4
                            spacing: 4

                            Label {
                                text: name
                                color: "#CCCCCC"
                                font.pixelSize: 10
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            Rectangle {
                                width: 14
                                height: 14
                                radius: 2
                                color: "transparent"
                                visible: tabModel.count > 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "×"
                                    color: "#888888"
                                    font.pixelSize: 10
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (tabModel.count > 1)
                                            tabModel.removeTab(index)
                                    }
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: tabModel.setActive(index)
                        }
                    }
                }

                // Add tab button
                Rectangle {
                    width: 22
                    height: 22
                    radius: 3
                    color: mouseAddTab.containsMouse ? "#3A3A3A" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "+"
                        color: "#888888"
                        font.pixelSize: 14
                    }

                    MouseArea {
                        id: mouseAddTab
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: tabModel.addTab("Canvas " + (tabModel.count + 1))
                    }
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: {
                        var count = effectiveGraphModel ? effectiveGraphModel.qmlNodeIds().length : 0
                        return count + " nodes"
                    }
                    color: "#666666"
                    font.pixelSize: 10
                    anchors.rightMargin: 8
                }
            }
        }
    }

    // ── Add Node Popup (Shift+A) ──────────────────────────────
    AddNodePopup {
        id: addNodePopup
        graphModel: effectiveGraphModel
        undoManager: effectiveUndoManager
        anchors.centerIn: parent
        width: Math.min(400, root.width * 0.8)
        height: Math.min(500, root.height * 0.7)

        visible: false
        modal: true
    }

    // ── Properties Panel (sidebar overlay for selected node) ──
    Rectangle {
        id: propertiesPanel
        width: 220
        anchors.top: parent.top
        anchors.topMargin: 36
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        color: "#252525"
        visible: root.selectedNodeId !== ""

        property string nodeId: ""

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Label {
                text: "Properties"
                color: "#CCCCCC"
                font.pixelSize: 12
                font.bold: true
                padding: 10
                Layout.fillWidth: true
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#3A3A3A"
            }

            PropertiesPanel {
                Layout.fillWidth: true
                Layout.fillHeight: true
                graphModel: effectiveGraphModel
                undoManager: effectiveUndoManager
                nodeId: propertiesPanel.nodeId
            }
        }
    }

    // ── File Dialogs ──────────────────────────────────────────
    FileDialog {
        id: openFileDialog
        title: "Open Graph"
        nameFilters: ["Node Graph Files (*.json)", "All Files (*)"]
        onAccepted: {
            if (effectiveGraphModel) {
                var file = openFileDialog.selectedFile.toString()
                file = file.replace(/^(file:\/{2})/, "")
                effectiveGraphModel.qmlLoadFromFile(file)
            }
        }
    }

    FileDialog {
        id: saveAsDialog
        title: "Save Graph As"
        nameFilters: ["Node Graph Files (*.json)", "All Files (*)"]
        fileMode: FileDialog.SaveFile
        defaultSuffix: "json"
        onAccepted: {
            if (effectiveGraphModel) {
                var file = saveAsDialog.selectedFile.toString()
                file = file.replace(/^(file:\/{2})/, "")
                effectiveGraphModel.qmlSaveToFile(file)
            }
        }
    }

    // ── Canvas Tab Model ──────────────────────────────────────
    ListModel {
        id: tabModel

        property int activeIndex: 0

        function addTab(name) {
            append({ name: name, active: count === 0 })
            if (count === 1) setActive(0)
        }

        function setActive(index) {
            for (var i = 0; i < count; i++) {
                setProperty(i, "active", i === index)
            }
            activeIndex = index
        }

        function removeTab(index) {
            ListModel.prototype.remove.call(this, index, 1)
            if (activeIndex >= count) activeIndex = count - 1
            if (count > 0) setActive(activeIndex)
        }

        Component.onCompleted: addTab("Canvas 1")
    }

    // ── Keyboard Shortcuts ────────────────────────────────────
    Shortcut {
        sequences: [StandardKey.Undo]
        onActivated: { if (effectiveUndoManager) effectiveUndoManager.undo() }
    }

    Shortcut {
        sequences: [StandardKey.Redo]
        onActivated: { if (effectiveUndoManager) effectiveUndoManager.redo() }
    }

    Shortcut {
        sequence: "Shift+A"
        onActivated: addNodePopup.open()
    }

    Shortcut {
        sequences: [StandardKey.Delete]
        onActivated: {
            if (root.selectedNodeId && effectiveUndoManager) {
                effectiveUndoManager.qmlRemoveNode(root.selectedNodeId)
                root.selectedNodeId = ""
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+D"
        onActivated: {
            if (root.selectedNodeId && effectiveUndoManager) {
                var info = effectiveGraphModel.qmlNodeInfo(root.selectedNodeId)
                if (info && info.type)
                    effectiveUndoManager.qmlAddNode(info.type, info.x + 30, info.y + 30)
            }
        }
    }

    Shortcut {
        sequence: "Shift+D"
        onActivated: {
            if (root.selectedNodeId && effectiveUndoManager) {
                var info = effectiveGraphModel.qmlNodeInfo(root.selectedNodeId)
                if (info && info.type)
                    effectiveUndoManager.qmlAddNode(info.type, info.x + 30, info.y + 30)
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+N"
        onActivated: {
            effectiveGraphModel.clear()
            if (effectiveUndoManager) effectiveUndoManager.clear()
        }
    }

    Shortcut {
        sequence: "Ctrl+O"
        onActivated: openFileDialog.open()
    }

    Shortcut {
        sequence: "Ctrl+S"
        onActivated: saveCurrentGraph()
    }

    Shortcut {
        sequence: "Ctrl+Shift+S"
        onActivated: saveAsDialog.open()
    }

    Shortcut {
        sequence: "Shift+V"
        onActivated: {
            if (root.selectedNodeId)
                canvasItem.fitToNode(root.selectedNodeId)
        }
    }

    Shortcut {
        sequence: "F"
        onActivated: canvasItem.fitToView()
    }

    // ── Helper Functions ──────────────────────────────────────
    function computeGraph() {
        if (!effectiveGraphModel) return
        // Data flow propagates automatically via DataFlowEngine.
        // This method is kept as a trigger for future manual evaluation.
    }

    function saveCurrentGraph() {
        if (!effectiveGraphModel) return
        effectiveGraphModel.qmlSaveToFile("graph_" + Date.now() + ".json")
    }

    // Expose internal components
    function openAddNodePopup() { addNodePopup.open() }
    function fitToView() { canvasItem.fitToView() }
}
