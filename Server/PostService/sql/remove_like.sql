WITH unlike_attempt AS (
    DELETE FROM likes
    WHERE post_id = $1 AND user_id = $2
    RETURNING post_id
)
SELECT likes_count 
FROM posts 
WHERE post_id = $1;