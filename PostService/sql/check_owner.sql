SELECT EXISTS(
    SELECT 1 
    FROM posts 
    WHERE post_id = $1 AND user_id = $2
) AS is_owner;