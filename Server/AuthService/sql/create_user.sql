INSERT INTO users(first_name, last_name, email, password_hash)
VALUES ($1, $2, $3, $4)
ON CONFLICT (email) DO NOTHING