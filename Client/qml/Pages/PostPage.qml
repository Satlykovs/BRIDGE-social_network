import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import Client.Components
import Client

Rectangle {
    id: root
    anchors.fill: parent
    visible: false
    color: "#1E1E2E"

    property bool isLoading: postManager ? postManager.isLoading : false

    // Функция преобразования JSON поста в данные модели
    function postToModelData(post) {
        if (!post) {
            console.warn("Post is null or undefined");
            return null;
        }

        // Форматируем дату
        let formattedDate = "";
        try {
            const date = new Date(post.createdAt || post.date || "");
            if (!isNaN(date.getTime())) {
                formattedDate = date.toLocaleDateString(Qt.locale("ru_RU"), "yyyy-MM-dd");
            }
        } catch (e) {
            console.warn("Date formatting error:", e);
        }

        return {
            id: String(post.postId || post.id || ""),
            text: String(post.content || post.text || ""),
            imageUrl: String(post.imageUrl || ""),
            likes: Number(post.likesCount || post.likes || 0),
            date: formattedDate,
            liked: Boolean(post.likedByCurrentUser || post.liked || false),
            isMine: Boolean(post.isOwnedByCurrentUser || post.isMine || false)
        };
    }

    // Модель для хранения постов
    property ListModel postsModel: ListModel {}

    // Диалог подтверждения удаления
    ConfirmDeleteDialog {
        id: confirmDeleteDialog
        onConfirmed: {
            if (postManager) {
                postManager.deletePost(postId);
            }
        }
    }

    // Диалог редактирования/создания поста
    EditPostDialog {
        id: editPostDialog
        onSaveRequested: {
            if (postManager) {
                if (isNewPost) {
                    postManager.createPost(newText, newImageUrl);
                } else {
                    postManager.updatePost(postId, newText, newImageUrl);
                }
            }
        }
    }

    Column {
        anchors.fill: parent
        spacing: 0

        PostHeader {
            id: header
            width: parent.width
            onBackClicked: root.visible = false
        }

        PostList {
            width: parent.width
            height: parent.height - header.height
            model: root.postsModel
            cacheBuffer: 400

            delegate: Post {
                width: Math.min(340, PostList.view.width - 40)
                postData: model

                onEditClicked: {
                    editPostDialog.isNewPost = false;
                    editPostDialog.postId = postData.id;
                    editPostDialog.postText = postData.text;
                    editPostDialog.imageUrl = postData.imageUrl;
                    editPostDialog.open();
                }

                onDeleteClicked: {
                    confirmDeleteDialog.postId = postData.id;
                    confirmDeleteDialog.open();
                }

                onLikeClicked: {
                    const newLiked = !postData.liked;
                    const newLikes = postData.likes + (newLiked ? 1 : -1);
                    postData.liked = newLiked;
                    postData.likes = newLikes;
                    emojiBounce.start();

                    if (root.postManager) {
                        root.postManager.toggleLike(postData.id, newLiked);
                    }
                }
            }
        }
    }

    // Кнопка добавления нового поста
    AddPostButton {
        editPostDialog: editPostDialog
    }

    // Индикатор загрузки
    BusyIndicator {
        anchors.centerIn: parent
        running: root.isLoading
        visible: running
        width: 48
        height: 48
    }

    // Обработчики сигналов от PostManager
    Connections {
        target: postManager

        onPostsLoaded: function(posts) {
            console.log("Posts received, count:", posts ? posts.length : 0);

            postsModel.clear();

            if (!posts || !posts.length) {
                console.warn("No posts received");
                return;
            }

            // Сначала собираем все данные
            var newPosts = [];
            for (var i = 0; i < posts.length; i++) {
                var modelData = postToModelData(posts[i]);
                if (modelData) {
                    newPosts.push(modelData);
                }
            }

            // Затем добавляем в модель
            for (i = 0; i < newPosts.length; i++) {
                postsModel.append(newPosts[i]);
            }
        }

        onPostCreated: function(post) {
            var modelData = postToModelData(post);
            if (modelData) {
                postsModel.append(modelData);
            }
        }

        onPostUpdated: function(post) {
            for (var i = 0; i < postsModel.count; i++) {
                if (postsModel.get(i).id === post.id) {
                    var modelData = postToModelData(post);
                    if (modelData) {
                        postsModel.set(i, modelData);
                    }
                    break;
                }
            }
        }

        onPostDeleted: function(postId) {
            for (var i = 0; i < postsModel.count; i++) {
                if (postsModel.get(i).id === postId) {
                    postsModel.remove(i);
                    break;
                }
            }
        }

        onPostLikeToggled: function(post) {
            for (var i = 0; i < postsModel.count; i++) {
                if (postsModel.get(i).id === post.id) {
                    var modelData = postToModelData(post);
                    if (modelData) {
                        postsModel.set(i, modelData);
                    }
                    break;
                }
            }
        }

        onErrorOccurred: function(errorMessage) {
            console.error("Post Error:", errorMessage);
            // TODO: Реализовать показ ошибки пользователю
        }
    }
}
