UPDATE user_profile
SET
    first_name = COALESCE(NULLIF($2, ''), first_name),
    last_name = COALESCE(NULLIF($3, ''), last_name),
    username = COALESCE(NULLIF($4, ''), username),
    avatar_url = COALESCE(NULLIF($5, ''), avatar_url)
WHERE id = $1
RETURNING *;