you will need to update node ui input for color, by default their will be a rectangle with the defautl color
when the rectangle is clicked the color wheel will show, users will then be able to select the color of their choice
and color picker tooo similar to blender layout for color picking.

To implement a color wheel in Qt6 QML, the most efficient approach is to use a ShaderEffect combined with Qt.hsva for color calculation, avoiding the performance overhead of Canvas or Image pixel manipulation. 

1. Shader-Based Color Wheel (Recommended)
This method renders a perfect color wheel using GLSL shaders and calculates the selected color based on the mouse angle relative to the center. 

import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: 300
    height: 300

    property real selectedHue: 0.0
    property color selectedColor: Qt.hsva(selectedHue, 1.0, 1.0, 1.0)

    ShaderEffect {
        id: colorWheel
        anchors.fill: parent
        property real ringWidth: 0.2 // Relative width of the wheel ring (0.0 to 0.5)
        
        fragmentShader: "
            varying highp vec2 qt_TexCoord0;
            uniform highp float qt_Opacity;
            uniform highp float ringWidth;

            vec3 hsv2rgb(vec3 c) {
                vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }

            void main() {
                highp vec2 coord = qt_TexCoord0 - vec2(0.5);
                highp float dist = length(coord);
                highp float ring = smoothstep(0, 0.01, -abs(dist - 0.5 + ringWidth) + ringWidth);
                
                // Calculate angle for hue (0.0 to 1.0)
                highp float angle = -atan(coord.x, coord.y) / 6.2831 + 0.5;
                if (angle < 0.0) angle += 1.0;
                
                gl_FragColor = vec4(hsv2rgb(vec3(angle, 1.0, 1.0)), 1.0) * ring;
            }
        "
    }

    MouseArea {
        anchors.fill: parent
        onClicked: handleColorSelection(mouseX, mouseY)
        onPositionChanged: handleColorSelection(mouseX, mouseY)

        function handleColorSelection(x, y) {
            // Convert mouse coordinates to 0.0-1.0 range
            var normX = x / parent.width;
            var normY = y / parent.height;
            
            // Calculate angle from center
            var centerX = 0.5;
            var centerY = 0.5;
            var dx = normX - centerX;
            var dy = centerY - normY; // Invert Y for standard polar coords
            
            var angle = -Math.atan2(dx, dy) / (2 * Math.PI) + 0.5;
            if (angle < 0) angle += 1.0;
            
            selectedHue = angle;
        }
    }

    // Visual indicator for selected color
    Rectangle {
        width: 50
        height: 50
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10
        color: selectedColor
        border.color: "black"
    }
}



###############
To implement a color wheel with opacity and saturation controls in Qt 6 QML, you should use QtQuick.Effects.MultiEffect or ShaderEffect instead of the deprecated QtGraphicalEffects module (like ColorOverlay), which was removed in Qt 6. 

1. Recommended Approach: ShaderEffect for the Wheel
The most performant and modern way to draw a color wheel in Qt 6 is using a ShaderEffect with a fragment shader. This allows you to calculate colors based on mouse angle (hue) and distance (saturation/value). 

import QtQuick
import QtQuick.Controls

Item {
    width: 300
    height: 400

    property vector4d colorHSVA: Qt.vector4d(0, 1, 1, 1) // Hue, Saturation, Value, Alpha

    // Color Wheel Component
    ShaderEffect {
        id: colorWheel
        width: 200
        height: 200
        anchors.horizontalCenter: parent.horizontalCenter
        property real hue: colorHSVA.x
        property real saturation: colorHSVA.y
        property real value: colorHSVA.z

        fragmentShader: "
            uniform highp float qt_Opacity;
            uniform highp float hue;
            uniform highp float saturation;
            uniform highp float value;
            varying highp vec2 qt_TexCoord0;

            vec3 hsv2rgb(vec3 c) {
                vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
            }

            void main() {
                highp vec2 coord = qt_TexCoord0 - vec2(0.5);
                highp float angle = atan(coord.y, coord.x) / 6.28318 + 0.5;
                highp float dist = length(coord);
                
                // Simple wheel: Hue is angle, Saturation is distance from center
                vec3 col = hsv2rgb(vec3(angle, dist, 1.0));
                gl_FragColor = vec4(col, qt_Opacity);
            }
        "
        
        MouseArea {
            anchors.fill: parent
            onPressed: {
                var coord = Qt.point(mouse.x / width, mouse.y / height);
                var angle = Math.atan2(coord.y - 0.5, coord.x - 0.5) / 6.28318 + 0.5;
                var dist = Math.sqrt(Math.pow(coord.x - 0.5, 2) + Math.pow(coord.y - 0.5, 2));
                colorHSVA = Qt.vector4d(angle, dist, colorHSVA.z, colorHSVA.w);
            }
        }
    }

    // Saturation Slider (Vertical)
    Column {
        anchors.top: colorWheel.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        
        Text { text: "Saturation" }
        Slider {
            width: 200
            from: 0.0; to: 1.0
            value: colorHSVA.y
            onValueChanged: {
                colorHSVA = Qt.vector4d(colorHSVA.x, value, colorHSVA.z, colorHSVA.w);
            }
        }
    }

    // Opacity Slider (Vertical)
    Column {
        anchors.top: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10
        
        Text { text: "Opacity" }
        Slider {
            width: 200
            from: 0.0; to: 1.0
            value: colorHSVA.w
            onValueChanged: {
                colorHSVA = Qt.vector4d(colorHSVA.x, colorHSVA.y, colorHSVA.z, value);
            }
        }
    }

    // Display Result
    Rectangle {
        anchors.bottom: parent.bottom
        width: 200
        height: 50
        color: Qt.rgba(colorHSVA.x, colorHSVA.y, colorHSVA.z, colorHSVA.w)
        anchors.horizontalCenter: parent.horizontalCenter
        border.color: "black"
    }
}
