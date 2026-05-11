import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import NodeEditor

Item {
    id: root

    property string selectedNodeId: ""

    readonly property var _nodeRegistry: typeof _nodeTypeRegistry !== "undefined" ? _nodeTypeRegistry : null

    readonly property var effectiveGraphModel: activeTab ? activeTab.graphModel : null
    readonly property var effectiveUndoManager: activeTab ? activeTab.undoManager : null

    property real defaultZoom: 1.0

    property alias canvas: canvasItem

    signal nodeSelected(string nodeId)
    signal nodeDeselected()

    // ── Tab Components ──────────────────────────────────────────
    Component {
        id: graphModelComp
        GraphModel {}
    }
    Component {
        id: undoManagerComp
        UndoManager {}
    }
    Component {
        id: engineComp
        DataFlowEngine {}
    }

    // ── Active Tab ──────────────────────────────────────────────
    property var activeTab: null

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
                            text: "New Tab"
                            onTriggered: tabModel.addTab("Canvas")
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
                        MenuItem {
                            text: "Save All"
                            onTriggered: saveAllGraphs()
                        }
                        MenuSeparator {}
                        MenuItem {
                            text: "Clear All"
                            onTriggered: {
                                if (activeTab) {
                                    activeTab.graphModel.clear()
                                    activeTab.undoManager.clear()
                                }
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
                    onClicked: {
                        if (activeTab) activeTab.engine.processAll()
                    }
                }
                ToolButton {
                    id: autoBtn
                    text: "Auto"
                    flat: true
                    checkable: true
                    font.pixelSize: 11
                    contentItem: Text { text: parent.text; color: parent.checked ? "#4CDF8B" : "#CCCCCC"; font.pixelSize: 11 }
                    background: Rectangle { color: parent.hovered ? "#3A3A3A" : "transparent"; radius: 3 }
                    onClicked: {
                        if (activeTab) activeTab.engine.autoCompute = autoBtn.checked
                    }
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
                    id: tabRepeater
                    model: tabModel

                    delegate: Rectangle {
                        id: tabDelegate
                        height: 22
                        width: 120
                        radius: 3
                        color: active ? "#3A3A3A" : "#2A2A2A"
                        border.color: active ? "#555555" : "transparent"
                        border.width: 1

                        property bool renaming: false

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 4
                            spacing: 4

                            Loader {
                                Layout.fillWidth: true
                                sourceComponent: tabDelegate.renaming ? renameInput : tabLabel
                            }

                            Rectangle {
                                width: 14
                                height: 14
                                radius: 2
                                color: "transparent"
                                visible: tabModel.count > 1

                                Text {
                                    anchors.centerIn: parent
                                    text: "\u00d7"
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

                        Component {
                            id: tabLabel
                            Label {
                                text: tabModel.get(index).name
                                color: "#CCCCCC"
                                font.pixelSize: 10
                                elide: Text.ElideRight
                            }
                        }

                        Component {
                            id: renameInput
                            TextInput {
                                text: tabModel.get(index).name
                                color: "#CCCCCC"
                                font.pixelSize: 10
                                focus: true
                                selectByMouse: true
                                onAccepted: {
                                    tabModel.renameTab(index, text)
                                    tabDelegate.renaming = false
                                }
                                onEditingFinished: {
                                    tabDelegate.renaming = false
                                }
                                Keys.onEscapePressed: {
                                    tabDelegate.renaming = false
                                }
                                Component.onCompleted: {
                                    selectAll()
                                    forceActiveFocus()
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            onClicked: function(mouse) {
                                if (mouse.button === Qt.RightButton) {
                                    tabContextMenu.index = index
                                    tabContextMenu.popup()
                                } else {
                                    tabModel.setActive(index)
                                }
                            }
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
                        onClicked: tabModel.addTab("Canvas")
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

            // ── Tab Context Menu ────────────────────────────────
            Menu {
                id: tabContextMenu
                property int index: -1

                MenuItem {
                    text: "Rename"
                    onTriggered: {
                        if (tabContextMenu.index >= 0) {
                            var delegate = tabRepeater.itemAt(tabContextMenu.index)
                            if (delegate) delegate.renaming = true
                        }
                    }
                }
                MenuItem {
                    text: "Open..."
                    onTriggered: {
                        if (tabContextMenu.index >= 0) {
                            tabContextMenuIndex = tabContextMenu.index
                            openFileDialog.open()
                        }
                    }
                }
                MenuItem {
                    text: "Export..."
                    onTriggered: {
                        if (tabContextMenu.index >= 0) {
                            exportTabIndex = tabContextMenu.index
                            saveAsDialog.open()
                        }
                    }
                }
                MenuItem {
                    text: "Close"
                    enabled: tabModel.count > 1
                    onTriggered: {
                        if (tabContextMenu.index >= 0 && tabModel.count > 1)
                            tabModel.removeTab(tabContextMenu.index)
                    }
                }
            }
        }
    }

    // ── Tab index trackers for context menu ─────────────────────
    property int tabContextMenuIndex: -1
    property int exportTabIndex: -1

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
            var targetIdx = tabContextMenuIndex >= 0 ? tabContextMenuIndex : tabModel.activeIndex
            var tab = tabModel.get(targetIdx)
            if (tab && tab.graphModel) {
                var file = openFileDialog.selectedFile.toString()
                file = file.replace(/^(file:\/{2})/, "")
                tab.graphModel.qmlLoadFromFile(file)
                // Set file path and rename tab to filename
                tab.filePath = file
                var parts = file.split("/")
                var fname = parts[parts.length - 1]
                if (fname.indexOf(".") > 0) fname = fname.substring(0, fname.lastIndexOf("."))
                tabModel.renameTab(targetIdx, fname)
                if (targetIdx === tabModel.activeIndex && canvasItem && canvasItem.gridCanvas) canvasItem.gridCanvas.requestPaint()
            }
            tabContextMenuIndex = -1
        }
    }

    FileDialog {
        id: saveAsDialog
        title: "Save Graph As"
        nameFilters: ["Node Graph Files (*.json)", "All Files (*)"]
        fileMode: FileDialog.SaveFile
        defaultSuffix: "json"
        onAccepted: {
            var targetIdx = exportTabIndex >= 0 ? exportTabIndex : tabModel.activeIndex
            var tab = tabModel.get(targetIdx)
            if (tab && tab.graphModel) {
                var file = saveAsDialog.selectedFile.toString()
                file = file.replace(/^(file:\/{2})/, "")
                tab.graphModel.qmlSaveToFile(file)
                tab.filePath = file
                var parts = file.split("/")
                var fname = parts[parts.length - 1]
                if (fname.indexOf(".") > 0) fname = fname.substring(0, fname.lastIndexOf("."))
                tabModel.renameTab(targetIdx, fname)
            }
            exportTabIndex = -1
        }
    }

    // ── Canvas Tab Model ──────────────────────────────────────
    ListModel {
        id: tabModel

        property int activeIndex: 0

        function addTab(name) {
            name = root.uniqueTabName(name)
            var g = graphModelComp.createObject(root)
            if (root._nodeRegistry) g.qmlCopyRegistryFrom(root._nodeRegistry)
            var u = undoManagerComp.createObject(root, {graphModel: g})
            var e = engineComp.createObject(root, {graphModel: g})
            append({
                name: name,
                filePath: "",
                active: count === 0,
                graphModel: g,
                undoManager: u,
                engine: e
            })
            if (count === 1) setActive(0)
        }

        function setActive(index) {
            if (index < 0 || index >= count) return
            for (var i = 0; i < count; i++)
                setProperty(i, "active", i === index)
            activeIndex = index
            root.activeTab = get(index)
            if (canvasItem && canvasItem.gridCanvas)
                canvasItem.gridCanvas.requestPaint()
            autoBtn.checked = root.activeTab.engine.autoCompute
        }

        function removeTab(index) {
            if (count <= 1) return
            var item = get(index)
            if (item.graphModel) item.graphModel.destroy()
            if (item.undoManager) item.undoManager.destroy()
            if (item.engine) item.engine.destroy()
            ListModel.prototype.remove.call(this, index, 1)
            if (activeIndex >= count) activeIndex = count - 1
            if (count > 0) setActive(activeIndex)
        }

        function renameTab(index, newName) {
            newName = root.uniqueTabName(newName)
            setProperty(index, "name", newName)
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
            if (activeTab) {
                activeTab.graphModel.clear()
                activeTab.undoManager.clear()
            }
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
        sequence: "Ctrl+W"
        onActivated: {
            if (tabModel.count > 1)
                tabModel.removeTab(tabModel.activeIndex)
        }
    }

    Shortcut {
        sequence: "Ctrl+Tab"
        onActivated: {
            var next = (tabModel.activeIndex + 1) % tabModel.count
            tabModel.setActive(next)
        }
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
    function uniqueTabName(base) {
        var existing = []
        for (var i = 0; i < tabModel.count; i++)
            existing.push(tabModel.get(i).name)
        if (existing.indexOf(base) < 0) return base
        var idx = 1
        while (existing.indexOf(base + "-" + idx) >= 0) idx++
        return base + "-" + idx
    }

    function saveCurrentGraph() {
        if (!activeTab) return
        var tab = tabModel.get(tabModel.activeIndex)
        if (!tab.filePath) {
            exportTabIndex = tabModel.activeIndex
            saveAsDialog.open()
            return
        }
        tab.graphModel.qmlSaveToFile(tab.filePath)
        var parts = tab.filePath.split("/")
        var fname = parts[parts.length - 1]
        if (fname.indexOf(".") > 0) fname = fname.substring(0, fname.lastIndexOf("."))
        tabModel.renameTab(tabModel.activeIndex, fname)
    }

    function saveAllGraphs() {
        for (var i = 0; i < tabModel.count; i++) {
            var tab = tabModel.get(i)
            if (!tab.filePath) {
                exportTabIndex = i
                saveAsDialog.open()
                return
            }
            tab.graphModel.qmlSaveToFile(tab.filePath)
            var parts = tab.filePath.split("/")
            var fname = parts[parts.length - 1]
            if (fname.indexOf(".") > 0) fname = fname.substring(0, fname.lastIndexOf("."))
            tabModel.renameTab(i, fname)
        }
    }

    // Expose internal components
    function openAddNodePopup() { addNodePopup.open() }
    function fitToView() { canvasItem.fitToView() }

    // ── Cleanup on destroy ────────────────────────────────────
    Component.onDestruction: {
        for (var i = 0; i < tabModel.count; i++) {
            var item = tabModel.get(i)
            if (item.graphModel) item.graphModel.destroy()
            if (item.undoManager) item.undoManager.destroy()
            if (item.engine) item.engine.destroy()
        }
    }
}
