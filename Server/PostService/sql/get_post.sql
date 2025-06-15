SELECT 
    p.*,
    EXISTS (
        SELECT 1 FROM likes l 
        WHERE l.post_id = p.post_id AND l.user_id = $2
    ) AS liked,
    (p.user_id = $current_user_id) AS isMine
FROM 
    posts p
WHERE 
    p.post_id = $1;