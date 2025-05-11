DELETE FROM posts
WHERE post_id = $1 AND author_id = $2
RETURNING post_id;
