#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include "NetworkManager.h"
#include "ProfileLoader.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("YourOrganization");
    QCoreApplication::setApplicationName("YourApp");

    QQmlApplicationEngine engine;

    NetworkManager networkManager;

    engine.rootContext()->setContextProperty("networkManager",  &networkManager);

    // Регистрируем наш новый класс как тип, чтобы QML мог создавать его экземпляры
    qmlRegisterType<ProfileLoader>("Client", 1, 0, "ProfileLoader");

    QQuickStyle::setStyle("Fusion");

    engine.loadFromModule("Client.Pages", "Main");

    networkManager.checkSavedTokens();

    return app.exec();
}
