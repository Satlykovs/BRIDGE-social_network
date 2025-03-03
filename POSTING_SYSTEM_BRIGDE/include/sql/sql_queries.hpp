#pragma once

namespace sql_queries {
namespace sql {

constexpr const char* kCreatePost = R"sql(
INSERT INTO posts (user_id, content) VALUES ($1, $2);
)sql";

constexpr const char* kDeletePost = R"sql(
DELETE FROM posts WHERE post_id = $1;
)sql";

constexpr const char* kEditPost = R"sql(
UPDATE posts SET content = $1 WHERE post_id = $2;
)sql";

constexpr const char* kFindPostById = R"sql(
SELECT post_id, user_id, content FROM posts WHERE post_id = $1;
)sql";

} // namespace sql
} // namespace sql_queries