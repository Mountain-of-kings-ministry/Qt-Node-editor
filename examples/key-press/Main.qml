import QtQuick
import QtQuick.Controls
import NodeEditor

Window {
    width: 1200
    height: 800
    visible: true
    title: "Key Press Visualizer"
    color: "#1E1E1E"

    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: function(event) {
            workspace.keyPressed(event.text, true)
        }
        Keys.onReleased: function(event) {
            workspace.keyPressed(event.text, false)
        }

        NodeEditorWorkspace {
            id: workspace
            anchors.fill: parent

            function keyPressed(key, pressed) {
                var model = graphModel
                if (!model) return
                var nodes = model.nodes
                for (var i = 0; i < nodes.length; i++) {
                    if (nodes[i].nodeType === "keyPress/capture") {
                        nodes[i].setKeyEvent(key, pressed)
                    }
                }
            }
        }
    }
}
