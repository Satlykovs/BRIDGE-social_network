SELECT 
    p.*,  -- Все поля из таблицы posts без изменений
    EXISTS (
        SELECT 1 FROM likes l 
        WHERE l.post_id = p.post_id AND l.user_id = $1
    ) AS liked,
    (p.user_id = $1) AS is_mine
FROM 
    posts p
WHERE 
    p.user_id = $2
ORDER BY 
    p.created_at DESC;