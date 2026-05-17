import QtQuick
import NodeEditor

Window {
    width: 1200
    height: 800
    visible: true
    title: "Random Value Display \u2014 Battle Test"
    color: "#1E1E1E"

    NodeEditorWorkspace {
        id: workspace
        anchors.fill: parent
        previewManager: _previewManager

        // Status overlay
        Rectangle {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 8
            width: 220
            height: 28
            color: "#AA000000"
            radius: 4
            z: 100

            Row {
                anchors.centerIn: parent
                spacing: 6
                Text {
                    text: "1ms drive"
                    color: "#4CDF8B"
                    font.pixelSize: 10
                    font.bold: true
                }
                Text {
                    text: "|"
                    color: "#555"
                    font.pixelSize: 10
                }
                Text {
                    text: "Timer \u2192 Random \u2192 LED"
                    color: "#999"
                    font.pixelSize: 10
                }
            }
        }
    }
}
