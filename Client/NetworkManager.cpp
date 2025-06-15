#include "NetworkManager.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QTimeZone>

NetworkManager::NetworkManager(QObject* parent) : QObject(parent),
    networkManager_(new QNetworkAccessManager(this)) {}

QDateTime NetworkManager::parseDateTime(const QString& dateTimeStr) {
    QString simplified = dateTimeStr.left(23);
    return QDateTime::fromString(simplified, "yyyy-MM-ddTHH:mm:ss.zzz");
}

void NetworkManager::sendRequest(const QUrl& url, HttpMethod method,
                                 QObject* receiver, const char* replySlot,
                                 const QJsonObject& body,
                                 bool needsAuth,
                                 const QVariant& userData)
{
    sendRequestInternal(url, method, receiver, replySlot,
                        body, needsAuth, userData, 0);
}

// Приватная версия с полной логикой
void NetworkManager::sendRequestInternal(const QUrl& url, HttpMethod method,
                                         QObject* receiver, const char* replySlot,
                                         const QJsonObject& body,
                                         bool needsAuth,
                                         const QVariant& userData,
                                         int retryCount)
{
    qDebug() << url;

    const int MAX_RETRIES = 1;
    const int RETRY_DELAY_MS = 500;

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (needsAuth && !m_tokenData.isAuthValid()) {
        if (retryCount >= MAX_RETRIES) {
            emit authRequired();
            return;
        }

        if (m_tokenData.isRefreshValid()) {
            refreshTokens();

            QMetaObject::Connection* connection = new QMetaObject::Connection;
            *connection = connect(this, &NetworkManager::tokenRefreshSuccess, this,
                                  [=]() {
                                      disconnect(*connection);
                                      delete connection;

                                      QTimer::singleShot(RETRY_DELAY_MS, this, [=]() {
                                          sendRequestInternal(url, method, receiver, replySlot,
                                                              body, true, userData, retryCount + 1);
                                      });
                                  }, Qt::SingleShotConnection);

            connect(this, &NetworkManager::tokenRefreshFailed, this,
                    [=]() {
                        disconnect(*connection);
                        delete connection;
                        emit authRequired();
                    }, Qt::SingleShotConnection);

            return;
        } else {
            emit authRequired();
            return;
        }
    }

    if (needsAuth) {
        request.setRawHeader("Authorization", "Bearer " + m_tokenData.authToken.toUtf8());
    }

    QNetworkReply* reply = nullptr;
    QByteArray requestBody = QJsonDocument(body).toJson();

    switch (method) {
    case HttpMethod::Get:    reply = networkManager_->get(request); break;
    case HttpMethod::Post:   reply = networkManager_->post(request, requestBody); break;
    case HttpMethod::Put:    reply = networkManager_->put(request, requestBody); break;
    case HttpMethod::Patch:  reply = networkManager_->sendCustomRequest(request, "PATCH", requestBody); break;
    case HttpMethod::Delete: reply = networkManager_->deleteResource(request); break;
    }
    if (reply) {
        connect(reply, &QNetworkReply::finished, [=]() {
            QScopedPointer<QNetworkReply> replyPtr(reply);

            if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401 &&
                needsAuth &&
                retryCount < MAX_RETRIES) {

                QTimer::singleShot(RETRY_DELAY_MS, this, [=]() {
                    sendRequestInternal(url, method, receiver, replySlot,
                                        body, true, userData, retryCount + 1);
                });
                return;
            }

            if (userData.isValid()) {
                QMetaObject::invokeMethod(receiver, replySlot,
                                          Q_ARG(QNetworkReply*, reply),
                                          Q_ARG(QVariant, userData));
            } else {
                QMetaObject::invokeMethod(receiver, replySlot,
                                          Q_ARG(QNetworkReply*, reply));
            }
        });

        connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError code) {
            qWarning() << "Network error occurred:" << code << reply->errorString();
        });
    }
}

void NetworkManager::registerUser(const QString& email, const QString& password) {
    QUrl url("http://localhost:8080/auth/register");
    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    // Указываем полную сигнатуру слота с параметром
    sendRequest(url, HttpMethod::Post, this,
                "onRegistrationFinished",
                json, false);
}

// Исправленная реализация с параметром
void NetworkManager::onRegistrationFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
            emit registrationSuccess();
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QString errorMsg = doc.object().value("message").toString("Server error");
            emit registrationFailed(errorMsg);
        }
    } else {
        emit registrationFailed("Ooops! Something went wrong");
    }
    reply->deleteLater();
}

void NetworkManager::loginUser(const QString& email, const QString& password) {
    QUrl url("http://localhost:8080/auth/login");
    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    // Указываем полную сигнатуру слота с параметром
    sendRequest(url, HttpMethod::Post, this,
                "onLoginFinished",
                json, false);
}

// Исправленная реализация с параметром
void NetworkManager::onLoginFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString authHeader = reply->rawHeader("Authorization");
        QString refreshToken = reply->rawHeader("X-Refresh-Token");


        if (!authHeader.isEmpty() && authHeader.startsWith("Bearer ")) {
            QString authToken = authHeader.mid(7);
            if (!authToken.isEmpty() && !refreshToken.isEmpty()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject json = doc.object();

                QDateTime authExpiry = parseDateTime(json["acсess-token-exp"].toString());
                QDateTime refreshExpiry = parseDateTime(json["refresh-token-exp"].toString());
                qDebug() << authToken << authExpiry;
                qDebug() << refreshToken << refreshExpiry;

                saveTokens(authToken, refreshToken, authExpiry, refreshExpiry);
                emit loginSuccess();
                reply->deleteLater();
                return;
            }
        }
        emit loginFailed("Invalid response from server");
    } else {
        emit loginFailed(reply->errorString());
    }
    reply->deleteLater();
}

void NetworkManager::refreshTokens() {
    if (!m_tokenData.isRefreshValid()) {
        emit tokenRefreshFailed();
        return;
    }

    QUrl url("http://localhost:8080/auth/refresh");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Refresh-Token", m_tokenData.refreshToken.toUtf8());

    auto reply = networkManager_->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onRefreshFinished(reply);
    });
}

// Исправленная реализация с параметром
void NetworkManager::onRefreshFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString authHeader = reply->rawHeader("Authorization");
        QString newRefreshToken = reply->rawHeader("X-Refresh-Token");

        if (!authHeader.isEmpty() && authHeader.startsWith("Bearer ")) {
            QString authToken = authHeader.mid(7);

            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject json = doc.object();

            QDateTime authExpiry = parseDateTime(json["access-token-exp"].toString());
            QDateTime refreshExpiry = parseDateTime(json["refresh-token-exp"].toString());

            saveTokens(authToken, newRefreshToken, authExpiry, refreshExpiry);
            emit tokenRefreshSuccess();
        }
    } else {
        emit tokenRefreshFailed();
        clearTokens();
    }
    reply->deleteLater();
}

void NetworkManager::checkSavedTokens() {
    QSettings settings;
    m_tokenData.authToken = settings.value("authToken").toString();
    m_tokenData.refreshToken = settings.value("refreshToken").toString();
    m_tokenData.authExpiry = QDateTime::fromString(settings.value("authTokenExp").toString(), Qt::ISODate);
    m_tokenData.authExpiry.setTimeZone(QTimeZone::utc());
    m_tokenData.refreshExpiry = QDateTime::fromString(settings.value("refreshTokenExp").toString(), Qt::ISODate);
    m_tokenData.refreshExpiry.setTimeZone(QTimeZone::utc());

    if (m_tokenData.isAuthValid()) {
        emit loginSuccess();
        qDebug() << "Immideately login";
    } else if (m_tokenData.isRefreshValid()) {
        refreshTokens();
        qDebug() << "Refresh then login";
        emit loginSuccess();
    } else {
        clearTokens();
    }
}

void NetworkManager::logout() {
    clearTokens();
    emit loggedOut();
}

void NetworkManager::saveTokens(const QString& authToken, const QString& refreshToken,
                                const QDateTime& authExpiry, const QDateTime& refreshExpiry) {
    m_tokenData = {authToken, refreshToken, authExpiry, refreshExpiry};
    m_tokenData.authExpiry.setTimeZone(QTimeZone::utc());
    m_tokenData.refreshExpiry.setTimeZone(QTimeZone::utc());

    QSettings settings;
    settings.setValue("authToken", authToken);
    settings.setValue("refreshToken", refreshToken);
    settings.setValue("authTokenExp", authExpiry.toString(Qt::ISODate));
    settings.setValue("refreshTokenExp", refreshExpiry.toString(Qt::ISODate));
}

void NetworkManager::clearTokens() {
    m_tokenData.clear();

    QSettings settings;
    settings.remove("authToken");
    settings.remove("refreshToken");
    settings.remove("authTokenExp");
    settings.remove("refreshTokenExp");
}
