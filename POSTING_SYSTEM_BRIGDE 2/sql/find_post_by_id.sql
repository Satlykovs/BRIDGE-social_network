SELECT post_id, user_id, content
FROM posts 
WHERE post_id = $1;