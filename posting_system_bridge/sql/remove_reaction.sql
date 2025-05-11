DELETE FROM reactions
WHERE post_id = $1 AND user_id = $2
RETURNING post_id;
