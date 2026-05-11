import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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

    function syncFromModel() {
        if (!graphModel || !nodeId) return
        nodeInfo = graphModel.qmlNodeInfo(nodeId)
        if (nodeInfo && nodeInfo.x !== undefined) {
            root.x = nodeInfo.x
            root.y = nodeInfo.y
        }
    }

    Component.onCompleted: syncFromModel()

    Connections {
        target: graphModel
        function onQmlNodePositionChanged(id) {
            if (id === root.nodeId) {
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

        MouseArea {
            id: dragArea
            anchors.fill: parent
            cursorShape: Qt.OpenHandCursor

            property real dragStartX: 0
            property real dragStartY: 0
            property real mouseStartX: 0
            property real mouseStartY: 0

            onPressed: function(mouse) {
                cursorShape = Qt.ClosedHandCursor
                dragStartX = root.x
                dragStartY = root.y
                mouseStartX = mouse.x
                mouseStartY = mouse.y
                var canvas = findCanvas()
                if (canvas) canvas.selectNode(root.nodeId)
            }

            onPositionChanged: function(mouse) {
                if (!pressed) return
                var canvas = findCanvas()
                var z = canvas ? canvas.zoom : 1.0
                root.x = dragStartX + (mouse.x - mouseStartX) / z
                root.y = dragStartY + (mouse.y - mouseStartY) / z
                if (root.graphModel)
                    root.graphModel.qmlSetNodePosition(root.nodeId, root.x, root.y)
            }

            onReleased: function(mouse) {
                cursorShape = Qt.OpenHandCursor
                if (root.undoManager
                        && (Math.abs(root.x - dragStartX) > 1 || Math.abs(root.y - dragStartY) > 1))
                    root.undoManager.qmlMoveNode(root.nodeId, dragStartX, dragStartY, root.x, root.y)
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
        height: inputColumn.height + outputColumn.height + 20
        radius: 6
        color: "#2A2A2A"
        border.color: root.isSelected() ? "#00B4FF" : "#3A3A3A"
        border.width: root.isSelected() ? 2 : 1

        MouseArea {
            anchors.fill: parent
            onClicked: function(mouse) {
                var canvas = findCanvas()
                if (!canvas) return
                if (mouse.modifiers & Qt.ControlModifier)
                    canvas.toggleNodeSelection(root.nodeId)
                else
                    canvas.selectNode(root.nodeId)
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

                Item {
                    id: inputDelegate
                    width: inputColumn.width
                    height: 16

                    property string dataOldVal: ""

                    Port {
                        id: portItem
                        graphModel: root.graphModel
                        nodeId: root.nodeId
                        portName: modelData
                        isInput: true
                        portType: (nodeInfo.inputPortTypes && nodeInfo.inputPortTypes[index]) || 0
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                    }

                    Text {
                        text: modelData
                        color: "#CCCCCC"
                        font.pixelSize: 11
                        anchors.left: portItem.right
                        anchors.leftMargin: 6
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TextField {
                        id: inputField
                        height: 20
                        width: 70
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: TextInput.AlignRight
                        color: "#FFFFFF"
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
                            return val !== undefined && val !== null ? String(val) : ""
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
    }
}
