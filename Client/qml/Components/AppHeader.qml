import QtQuick

Column {
    spacing: 15

    FloatingLogo {
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        text: "BRIDGE"
        color: "#FFFFFF"
        font {
            pixelSize: 36       // Восстанавливаем размер
            family: "Montserrat" // Правильный шрифт
            weight: Font.Bold    // Жирное начертание
            letterSpacing: 2     // Межбуквенный интервал
        }
        anchors.horizontalCenter: parent.horizontalCenter // Центрируем
    }

    Text {
        text: "Where worlds connect"
        color: "#FFECD2"


        anchors.horizontalCenter: parent.horizontalCenter
    }
}
