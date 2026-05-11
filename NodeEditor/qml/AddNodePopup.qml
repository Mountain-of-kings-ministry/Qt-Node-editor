import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import NodeEditor

Popup {
    id: root

    property var graphModel: null
    property var undoManager: null

    modal: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        color: "#2A2A2A"
        radius: 8
        border.color: "#444444"
        border.width: 1
    }

    onOpened: {
        searchField.text = ""
        nodeList.model = buildFilteredModel("")
        searchField.forceActiveFocus()
        searchField.selectAll()
    }

    Component.onCompleted: {
        nodeList.model = buildFilteredModel("")
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        TextField {
            id: searchField
            Layout.fillWidth: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.topMargin: 8
            height: 32
            placeholderText: "Search nodes..."
            color: "#FFFFFF"
            font.pixelSize: 13
            selectByMouse: true

            background: Rectangle {
                color: "#3A3A3A"
                radius: 4
                border.color: searchField.activeFocus ? "#4A9EFF" : "#555555"
                border.width: 1
            }

            onTextChanged: nodeList.model = buildFilteredModel(text)
        }

        ListView {
            id: nodeList
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 4
            Layout.rightMargin: 4
            Layout.bottomMargin: 4
            clip: true
            spacing: 0

            section.property: "category"
            section.delegate: Rectangle {
                width: ListView.view.width
                height: 26
                color: "#333333"

                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    text: section
                    color: "#AAAAAA"
                    font.pixelSize: 10
                    font.bold: true
                }
            }

            delegate: Item {
                width: ListView.view.width - 8
                height: 28

                Rectangle {
                    anchors.fill: parent
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    radius: 4
                    color: mouseArea.containsMouse ? "#3A3A3A" : "transparent"

                    RowLayout {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 8

                        Rectangle {
                            width: 8
                            height: 8
                            radius: 2
                            color: modelData.color || "#4A9EFF"
                        }

                        Text {
                            text: modelData.name || modelData.type || ""
                            color: "#DDDDDD"
                            font.pixelSize: 11
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked: {
                            if (root.undoManager)
                                root.undoManager.qmlAddNode(modelData.type, 200 + Math.random() * 400, 200 + Math.random() * 300)
                            else if (root.graphModel)
                                root.graphModel.qmlAddNode(modelData.type, 200 + Math.random() * 400, 200 + Math.random() * 300)
                            root.close()
                        }
                    }
                }
            }
        }
    }

    function buildFilteredModel(filterText) {
        var result = []
        if (!graphModel) return result

        var allTypes = graphModel.qmlAllNodeTypes()
        var lowerFilter = filterText.toLowerCase()

        for (var i = 0; i < allTypes.length; i++) {
            var type = allTypes[i]
            if (filterText && type.toLowerCase().indexOf(lowerFilter) < 0)
                continue

            var info = getNodeInfo(type)
            result.push({
                type: type,
                name: info.name || type.split("/").pop(),
                category: info.category || "Other",
                color: info.color || "#4A9EFF"
            })
        }
        return result
    }

    function getNodeInfo(type) {
        if (!graphModel) return {}
        var categories = graphModel.qmlCategories()
        for (var i = 0; i < categories.length; i++) {
            var cat = categories[i]
            var nodes = graphModel.qmlNodesInCategory(cat.id)
            for (var j = 0; j < nodes.length; j++) {
                if (nodes[j] === type) {
                    var name = type.split("/").pop()
                    name = name.charAt(0).toUpperCase() + name.slice(1)
                    return { name: name, category: cat.displayName, color: cat.color }
                }
            }
        }
        var name = type.split("/").pop()
        name = name.charAt(0).toUpperCase() + name.slice(1)
        return { name: name, category: "", color: "#4A9EFF" }
    }
}
