UPDATE posts
SET post_text = $1
WHERE post_id = $2 AND author_id = $3
RETURNING post_id;
