CREATE TABLE IF NOT EXISTS refresh_tokens(
token_id SERIAL PRIMARY KEY,
user_id INT REFERENCES users(id) ON DELETE CASCADE,
token TEXT NOT NULL,
expires_at TIMESTAMP NOT NULL
);

