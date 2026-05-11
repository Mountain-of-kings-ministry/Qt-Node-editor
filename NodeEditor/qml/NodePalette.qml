import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import NodeEditor

Item {
    id: root

    property var graphModel: null
    property var undoManager: null
    property var nodeTypes: ["Input", "Add", "Multiply", "Output"]
    property var colorMap: ({
        "Input": "#4CDF8B",
        "Add": "#FF9F43",
        "Multiply": "#4A9EFF",
        "Output": "#FF6B6B"
    })

    ColumnLayout {
        anchors.fill: parent
        spacing: 4

        Label {
            text: "Nodes"
            color: "#CCCCCC"
            font.pixelSize: 14
            font.bold: true
            padding: 10
            Layout.fillWidth: true
        }

        Repeater {
            model: root.nodeTypes

            Item {
                id: wrapper
                width: ListView.view ? ListView.view.width : parent.width
                height: 36

                Pane {
                    id: paletteItem
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    padding: 0
                    implicitHeight: 32

                    background: Rectangle {
                        color: ma.containsMouse ? "#3A3A3A" : "#2A2A2A"
                        radius: 4
                        border.color: ma.drag.active ? "#00B4FF" : "transparent"
                        border.width: ma.drag.active ? 2 : 0
                    }

                    RowLayout {
                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 8

                        Rectangle {
                            width: 10
                            height: 10
                            radius: 3
                            color: root.colorMap[modelData] || "#4A9EFF"
                        }

                        Label {
                            text: modelData
                            color: "#DDDDDD"
                            font.pixelSize: 12
                        }
                    }

                    Drag.keys: ["nodeeditor/nodetype"]
                    Drag.mimeData: { "nodeeditor/nodetype": modelData }

                    MouseArea {
                        id: ma
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        property bool wasDragged: false
                        property int startX: 0
                        property int startY: 0

                        onPressed: function(mouse) {
                            wasDragged = false
                            startX = mouse.x
                            startY = mouse.y
                        }

                        onMouseXChanged: function(mouse) {
                            if (!pressed || wasDragged) return
                            var dist = Math.abs(mouse.x - startX) + Math.abs(mouse.y - startY)
                            if (dist > 15) {
                                wasDragged = true
                                paletteItem.Drag.active = true
                            }
                        }

                        onReleased: function(mouse) {
                            if (wasDragged) {
                                paletteItem.Drag.active = false
                                paletteItem.Drag.drop()
                            } else {
                                root.addNode(modelData)
                            }
                        }
                    }
                }
            }
        }
    }

    function addNode(nodeType) {
        if (root.undoManager)
            root.undoManager.qmlAddNode(nodeType, 100 + Math.random() * 300, 100 + Math.random() * 200)
        else if (root.graphModel)
            root.graphModel.qmlAddNode(nodeType, 100 + Math.random() * 300, 100 + Math.random() * 200)
    }
}
