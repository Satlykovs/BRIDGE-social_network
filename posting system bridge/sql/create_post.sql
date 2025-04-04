INSERT INTO posts (author_id, post_text)
VALUES ($1, $2)
RETURNING post_id, author_id AS user_id, post_text AS content, created_at;
