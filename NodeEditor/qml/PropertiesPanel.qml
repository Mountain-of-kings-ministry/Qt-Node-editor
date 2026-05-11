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
    readonly property var _emptyList: []

    onNodeIdChanged: {
        if (nodeId && graphModel)
            nodeInfo = graphModel.qmlNodeInfo(nodeId)
        else
            nodeInfo = ({})
    }

    Connections {
        target: graphModel
        function onQmlNodeDataChanged(id, key) {
            if (id === root.nodeId)
                root.nodeInfo = graphModel.qmlNodeInfo(root.nodeId)
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
                        width: detailColumn.width
                        height: 24
                        property string dataOldVal: ""

                        Label {
                            text: modelData + ":"
                            color: "#AAAAAA"
                            font.pixelSize: 11
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                        }

                        TextField {
                            height: 22
                            width: 80
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
                    }
                }

                // Output port data
                Repeater {
                    model: root.nodeInfo.outputPorts || root._emptyList

                    Item {
                        width: detailColumn.width
                        height: 20

                        Label {
                            text: modelData + " (output)"
                            color: "#888888"
                            font.pixelSize: 11
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                        }

                        Label {
                            text: {
                                if (!root.graphModel || !root.nodeId) return ""
                                var val = root.graphModel.qmlNodeData(root.nodeId, modelData)
                                return val !== undefined && val !== null ? String(val) : ""
                            }
                            color: "#AAAAAA"
                            font.pixelSize: 11
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                        }
                    }
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
