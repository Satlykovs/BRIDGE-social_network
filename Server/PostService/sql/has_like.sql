SELECT EXISTS(
    SELECT 1 FROM likes 
    WHERE post_id = $1 AND user_id = $2
) AS has_like;