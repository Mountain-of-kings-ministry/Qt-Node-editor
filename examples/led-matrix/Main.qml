import QtQuick
import NodeEditor

Window {
    width: 1200
    height: 800
    visible: true
    title: "LED Matrix Display"
    color: "#1E1E1E"

    NodeEditorWorkspace {
        anchors.fill: parent
    }
}
