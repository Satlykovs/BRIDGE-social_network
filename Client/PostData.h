// postdata.h
#pragma once
#include <QString>
#include <QDateTime>

struct PostData {
    int postId;
    int userId;
    QString text;
    QString imageUrl;
    QDateTime createdAt;
    int likesCount;
    bool liked;
    bool isMine;

    // Конвертация в QVariantMap для QML
    QVariantMap toVariantMap() const {
        return {
            {"id", postId},
            {"userId", userId},
            {"text", text},
            {"imageUrl", imageUrl},
            {"likes", likesCount},
            {"liked", liked},
            {"isMine", isMine},
            {"date", createdAt.toString("yyyy-MM-dd")}
        };
    }
};
