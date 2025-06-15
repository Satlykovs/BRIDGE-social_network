CREATE TABLE IF NOT EXISTS posts(
	post_id SERIAL PRIMARY KEY,
    user_id INT,
    post_text TEXT NOT NULL,
    image_url TEXT,
    likes_count INT DEFAULT 0,
    created_at TIMESTAMPTZ DEFAULT NOW()
);