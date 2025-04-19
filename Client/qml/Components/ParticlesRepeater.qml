// ParticlesRepeater.qml
import QtQuick

Item {
    anchors.fill: parent

    // Настройки
    property int particleCount: 30 // Оптимальное количество
    property real speedVariation: 2000 // Разброс скорости (мс)

    // Аппаратное ускорение
    layer.enabled: true
    layer.smooth: true

    Repeater {
        model: particleCount

        delegate: Rectangle {
            id: particle
            width: 2 + Math.random() * 4
            height: width
            radius: width/2
            color: Qt.rgba(1, 1, 1, 0.15)
            opacity: 0

            // Начальные координаты
            x: Math.random() * (parent.width - width)
            y: parent.height + 50

            // Объединенная анимация
            SequentialAnimation {
                id: anim
                running: true
                loops: Animation.Infinite

                // Появление и движение
                ParallelAnimation {
                    NumberAnimation {
                        target: particle
                        property: "y"
                        from: parent.height + 50
                        to: -50
                        duration: 3000 + Math.random() * speedVariation
                        easing.type: Easing.Linear
                    }


                }

                ScriptAction {
                    script: resetParticle()
                }
            }

            // Сброс позиции
            function resetParticle() {
                x = Math.random() * (parent.width - width)
                y = parent.height + 50
                opacity = 0.7
            }

            Component.onCompleted: anim.start()
        }
    }
}
