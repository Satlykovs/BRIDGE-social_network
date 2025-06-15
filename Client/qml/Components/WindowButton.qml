import QtQuick

Rectangle {
    id: button
    width: 46
    height: 32
    // Анимация цвета фона при наведении и нажатии
    color: area.pressed ? colorPressed : (area.containsMouse ? colorHover : colorDefault)

    // --- Свойства для настройки кнопки ---
    property color colorDefault: "transparent"
    property color colorHover: "rgba(255, 255, 255, 0.1)"
    property color colorPressed: "rgba(255, 255, 255, 0.05)"
    property color iconColor: area.containsMouse ? "white" : "#B0B0D0"

    // Это свойство будет принимать компонент с Canvas для отрисовки иконки
    property Component iconComponent

    // Сигнал о нажатии
    signal clicked

    // Loader загрузит и отобразит иконку, которую мы передадим
    Loader {
        id: iconLoader
        anchors.centerIn: parent
        width: 12
        height: 12
        sourceComponent: button.iconComponent
        // Это нужно, чтобы иконка могла получить доступ к iconColor
        onLoaded: item.parent = button
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: button.clicked()
    }

    // Плавный переход цвета
    Behavior on color { ColorAnimation { duration: 150 } }
}
