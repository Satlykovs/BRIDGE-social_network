#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include "NetworkManager.h"
#include "PostManager.h"
#include "PostData.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("YourOrganization");
    QCoreApplication::setApplicationName("YourApp");

    QQmlApplicationEngine engine;

    NetworkManager* networkManager = new NetworkManager(&app);
    PostManager* postManager = new PostManager(networkManager, &app);


    qRegisterMetaType<PostData>("PostData");
    qRegisterMetaType<QVector<PostData>>("QVector<PostData>");

    engine.rootContext()->setContextProperty("networkManager", networkManager);
    engine.rootContext()->setContextProperty("postManager", postManager);



    QQuickStyle::setStyle("Fusion");

    engine.loadFromModule("Client.Pages", "Main");

    (*networkManager).checkSavedTokens();

    return app.exec();
}
