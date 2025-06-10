DELETE FROM posts
WHERE post_id = $1 AND user_id = $2
RETURNING image_url;