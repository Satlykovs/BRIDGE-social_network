CREATE TABLE IF NOT EXISTS likes (
    post_id INT NOT NULL REFERENCES posts(post_id),
    user_id INT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (post_id,user_id)
);