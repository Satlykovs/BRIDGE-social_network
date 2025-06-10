WITH like_attempt AS (
    INSERT INTO likes (post_id, user_id)
    VALUES ($1, $2)
    ON CONFLICT (post_id, user_id) DO NOTHING
    RETURNING post_id
)
SELECT likes_count 
FROM posts 
WHERE post_id = $1;