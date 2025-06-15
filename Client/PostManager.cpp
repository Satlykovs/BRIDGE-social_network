#include "PostManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "PostData.h"


QVector<PostData> parsePosts(const QByteArray& jsonData) {
    QVector<PostData> posts;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (!doc.isArray()) {
        qWarning() << "Invalid posts data - not an array";
        return posts;
    }

    for (const QJsonValue& value : doc.array()) {
        QJsonObject obj = value.toObject();
        PostData post;

        post.postId = obj["postId"].toInt();
        post.userId = obj["userId"].toInt();
        post.text = obj["text"].toString();
        post.imageUrl = obj["imageUrl"].toString();
        post.createdAt = QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate);
        post.likesCount = obj["likesCount"].toInt();
        post.liked = obj["liked"].toBool();
        post.isMine = obj["isMine"].toBool();

        posts.append(post);
    }

    return posts;
}




PostManager::PostManager(NetworkManager* networkManager, QObject* parent)
    : QObject(parent), m_networkManager(networkManager), m_isLoading(false)
{
}

void PostManager::setLoading(bool loading) {
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}

bool PostManager::isLoading() const {
    return m_isLoading;
}

void PostManager::loadPosts(int userId) {
    setLoading(true);
    QUrl url(QString("http://localhost:8082/post/all/%1").arg(userId));
    m_networkManager->sendRequest(url, NetworkManager::HttpMethod::Get,
                                  this, "handlePostsResponse", {}, true);
}

void PostManager::createPost(int userId, const QString& text, const QString& imageUrl) {
    QJsonObject body;
    body["content"] = text;  // Только контент и imageUrl
    body["imageUrl"] = imageUrl;

    // userId передается в URL, а не в теле
    QUrl url(QString("http://localhost:8082/post/%1").arg(userId));
    m_networkManager->sendRequest(url, NetworkManager::HttpMethod::Post,
                                  this, "handleCreateResponse", body);
}

void PostManager::updatePost(int postId, const QString& text, const QString& imageUrl) {
    QJsonObject body;
    body["content"] = text;
    body["imageUrl"] = imageUrl;

    QUrl url(QString("http://localhost:8082/post/%1").arg(postId));
    m_networkManager->sendRequest(url, NetworkManager::HttpMethod::Patch,
                                  this, "handleUpdateResponse", body);
}

void PostManager::deletePost(int postId) {
    QUrl url(QString("http://localhost:8082/post/%1").arg(postId));
    m_networkManager->sendRequest(url, NetworkManager::HttpMethod::Delete,
                                  this, "handleDeleteResponse", {}, true,
                                  QVariant::fromValue(postId));
}

void PostManager::toggleLike(int postId) {
    // Всегда GET запрос без параметра liked
    QUrl url(QString("http://localhost:8082/post/%1/like").arg(postId));
    m_networkManager->sendRequest(url, NetworkManager::HttpMethod::Get,
                                  this, "handleToggleLikeResponse");
}

void PostManager::handlePostsResponse(QNetworkReply* reply) {
    setLoading(false);
    if (reply->error() == QNetworkReply::NoError) {
        QVector<PostData> posts = parsePosts(reply->readAll());
        qDebug() << "Parsed" << posts.size() << "posts";

        // Конвертируем в QVariantList для QML
        QVariantList postsList;
        for (const PostData& post : posts) {
            postsList.append(post.toVariantMap());
        }

        emit postsLoaded(postsList);
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

void PostManager::handleCreateResponse(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        emit postCreated(doc.object());
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

void PostManager::handleUpdateResponse(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        emit postUpdated(doc.object());
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

void PostManager::handleDeleteResponse(QNetworkReply* reply, int postId) {
    if (reply->error() == QNetworkReply::NoError) {
        emit postDeleted(postId);
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

void PostManager::handleToggleLikeResponse(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        emit postLikeToggled(doc.object());
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}




