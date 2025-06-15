CREATE TABLE participants (
    conversation_id INT NOT NULL REFERENCES conversations(id) ON DELETE CASCADE,
    user_id BIGINT NOT NULL,
    role VARCHAR(20) DEFAULT 'member',
    last_read_message_id BIGINT,
    joined_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
    left_at TIMESTAMPTZ,
    notifications_enabled BOOLEAN DEFAULT TRUE NOT NULL,
    PRIMARY KEY (conversation_id, user_id)
);

