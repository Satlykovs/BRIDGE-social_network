#ifndef POSTMANAGER_H
#define POSTMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include "NetworkManager.h"

class PostManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit PostManager(NetworkManager* networkManager, QObject* parent = nullptr);

    Q_INVOKABLE void loadPosts(int userId);
    Q_INVOKABLE void createPost(int userId, const QString& text, const QString& imageUrl = "");
    Q_INVOKABLE void updatePost(int postId, const QString& text, const QString& imageUrl = "");
    Q_INVOKABLE void deletePost(int postId);
    Q_INVOKABLE void toggleLike(int postId);

    bool isLoading() const;

signals:
    void postsLoaded(const QVariantList& posts);
    void postCreated(const QJsonObject& post);
    void postUpdated(const QJsonObject& post);
    void postDeleted(int postId);
    void postLikeToggled(const QJsonObject& post);
    void errorOccurred(const QString& errorMessage);
    void isLoadingChanged();

private slots:
    void handlePostsResponse(QNetworkReply* reply);
    void handleCreateResponse(QNetworkReply* reply);
    void handleUpdateResponse(QNetworkReply* reply);
    void handleDeleteResponse(QNetworkReply* reply, int postId);
    void handleToggleLikeResponse(QNetworkReply* reply);

private:
    void setLoading(bool loading);

    NetworkManager* m_networkManager;
    bool m_isLoading;
};

#endif // POSTMANAGER_H
