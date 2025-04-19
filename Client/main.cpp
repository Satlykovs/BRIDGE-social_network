#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include"NetworkManager.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("YourOrganization");
    QCoreApplication::setApplicationName("YourApp");

    QQmlApplicationEngine engine;

    NetworkManager networkManager;

    engine.rootContext()->setContextProperty("networkManager",  &networkManager);

    QQuickStyle::setStyle("Fusion");

    engine.loadFromModule("Client.Pages", "Main");

    networkManager.checkSavedTokens();


    return app.exec();
}
