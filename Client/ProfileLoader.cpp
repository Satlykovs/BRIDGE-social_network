#include "ProfileLoader.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

ProfileLoader::ProfileLoader(QObject *parent)
    : QObject(parent)
{
}

void ProfileLoader::setNetworkManager(NetworkManager *manager)
{
    m_networkManager = manager;
}

void ProfileLoader::loadProfile()
{
    if (!m_networkManager) {
        qWarning() << "ProfileLoader Error: NetworkManager is not set!";
        emit loadError("Internal application error.");
        return;
    }

    if (m_isLoading) return;

    m_isLoading = true;
    emit isLoadingChanged();

    QUrl profileUrl("http://localhost:8081/profile");

    m_networkManager->sendRequest(profileUrl,
                                  NetworkManager::HttpMethod::Get,
                                  this,
                                  "onProfileReply",
                                  QJsonObject(),
                                  true);
}

void ProfileLoader::onProfileReply(QNetworkReply* reply)
{
    m_isLoading = false;
    emit isLoadingChanged();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject()) {
            QJsonObject data = doc.object();
            m_firstName = data["firstName"].toString("First");
            m_lastName = data["lastName"].toString("Last");
            m_username = data["username"].toString("username");
            m_avatarUrl = QUrl(data["avatarUrl"].toString());

            qDebug() << "Profile loaded for" << m_username;
            emit dataChanged();
        } else {
            emit loadError("Invalid server response format.");
        }
    } else {
        emit loadError("Network Error: " + reply->errorString());
    }

    reply->deleteLater();
}
