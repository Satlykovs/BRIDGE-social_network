WITH old_data AS(
    SELECT avatar_url as old_url
    FROM user_profile
    WHERE user_id = $1
)
UPDATE user_profile
SET
    avatar_url = $2
WHERE user_id = $1
RETURNING (SELECT old_url FROM old_data);