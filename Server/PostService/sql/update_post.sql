WITH old_data AS (
    SELECT image_url AS old_image_url 
    FROM posts 
    WHERE post_id = $1 AND user_id = $2
)
UPDATE posts
SET
    post_text = $3,
    image_url = $4
WHERE 
    post_id = $1 
    AND user_id = $2
RETURNING (SELECT old_image_url FROM old_data);