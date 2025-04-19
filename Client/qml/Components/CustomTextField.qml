import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

TextField {
    property color placeholderColor: "#FFECD280"
    property color backgroundColor: "#301A3A60"

    color: "#FFECD2"
    font.pixelSize: 14
    height: 50
    placeholderTextColor: placeholderColor

    background: Rectangle {
        color: backgroundColor
        radius: 10
        border.color: parent.activeFocus ? "#4ECDC4" : "#6C5B7B"
        border.width: 1.5
    }
}
