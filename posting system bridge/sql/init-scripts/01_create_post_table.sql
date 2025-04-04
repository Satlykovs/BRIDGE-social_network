CREATE TABLE IF NOT EXISTS posts (
    post_id SERIAL PRIMARY KEY,
    author_id INT NOT NULL,
    post_text TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
