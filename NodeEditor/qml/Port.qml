import QtQuick
import NodeEditor

Item {
    id: root

    property var graphModel: null
    property string nodeId: ""
    property string portName: ""
    property bool isInput: false
    property int portType: 0
    property bool connected: false

    width: 14
    height: 14

    function findCanvas() {
        var p = root.parent
        while (p) {
            if (p.objectName && p.objectName.indexOf("NodeCanvas") === 0)
                return p
            p = p.parent
        }
        return null
    }

    function portColor() {
        switch (root.portType) {
        case 0:  return "#4A9EFF"   // Int
        case 1:  return "#4CDF8B"   // Float
        case 2:  return "#FF9F43"   // String
        case 3:  return "#FF6B6B"   // Bool
        case 4:  return "#A29BFE"   // Color
        case 5:  return "#888888"   // Generic
        case 6:  return "#F0DB4F"   // Double
        case 7:  return "#E17055"   // Vec2
        case 8:  return "#E17055"   // Vec3
        case 9:  return "#E17055"   // Vec4
        case 10: return "#00CEC9"   // Array
        case 11: return "#00CEC9"   // Map
        case 12: return "#00CEC9"   // JSON
        case 13: return "#FD79A8"   // Image
        case 14: return "#6C5CE7"   // AudioBuffer
        default: return "#888888"
        }
    }

    Rectangle {
        id: circle
        anchors.centerIn: parent
        width: 12
        height: 12
        radius: 6
        color: root.portColor()
        border.width: root.connected ? 2 : 0
        border.color: "#FFFFFF"

        Behavior on color { ColorAnimation { duration: 100 } }

        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: "transparent"
            border.width: 1
            border.color: Qt.lighter(root.portColor(), 1.5)
            opacity: ma.containsMouse ? 0.6 : 0
            Behavior on opacity { NumberAnimation { duration: 100 } }
        }
    }

    MouseArea {
        id: ma
        anchors.fill: parent
        anchors.margins: -4
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onPressed: function(mouse) {
            if (root.isInput) return
            var canvas = findCanvas()
            if (!canvas) return
            var portCenter = root.mapToItem(canvas, width / 2, height / 2)
            canvas.startConnection(root.nodeId, root.portName, portCenter.x, portCenter.y)
        }

        onReleased: function(mouse) {
            if (!root.isInput) return
            var canvas = findCanvas()
            if (!canvas) return
            canvas.endConnection(root.nodeId, root.portName)
        }
    }
}
