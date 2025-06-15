WITH old_data AS (
    SELECT image_url AS old_image_url 
    FROM posts 
    WHERE post_id = $1 AND user_id = $2
),
updated_post AS (
    UPDATE posts
    SET
        post_text = $3,
        image_url = $4
    WHERE 
        post_id = $1 
        AND user_id = $2
    RETURNING 
        *,
        EXISTS (
            SELECT 1 FROM likes 
            WHERE post_id = $1 AND user_id = $2
        ) AS liked,
        true AS is_mine
)
SELECT 
    up.*,
    od.old_image_url
FROM 
    updated_post up,
    old_data od;