CREATE TABLE IF NOT EXISTS reactions (
    post_id INT REFERENCES posts (post_id) ON DELETE CASCADE,
    user_id INT NOT NULL,
    reaction VARCHAR(8) NOT NULL CHECK (reaction IN ('like', 'dislike')),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (post_id, user_id)
);

CREATE INDEX idx_reactions_post ON reactions (post_id);
