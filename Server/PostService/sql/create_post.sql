WITH new_post AS (
    INSERT INTO posts (user_id, post_text, image_url)
    VALUES ($1, $2, $3)
    RETURNING *
)
SELECT 
    np.*,
    false AS liked,
    true AS is_mine
FROM new_post np;