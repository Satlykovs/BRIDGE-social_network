import QtQuick

Text {
    property alias text: linkText.text
    signal clicked

    id: linkText
    color: "#FFD93D"
    font: Theme.linkFont

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: parent.clicked()
        onEntered: parent.color = "#FFECD2"
        onExited: parent.color = "#FFD93D"
    }
}
