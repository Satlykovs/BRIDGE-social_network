INSERT INTO post_comments(post_id, author_id, content)
VALUES($1, $2, $3)
RETURNING id, created_at;