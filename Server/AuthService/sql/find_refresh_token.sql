SELECT * FROM refresh_tokens
WHERE token = $1 AND expires_at > NOW();