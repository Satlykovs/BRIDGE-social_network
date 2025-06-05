UPDATE user_profile
SET
    first_name = COALESCE(NULLIF($2, ''), first_name),
    last_name = COALESCE(NULLIF($3, ''), last_name),
    username = COALESCE(NULLIF($4, ''), username)
WHERE user_id = $1
RETURNING *;