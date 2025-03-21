INSERT INTO posts (user_id, content)
VALUES ($1, $2)
RETURNING post_id, user_id, content;
