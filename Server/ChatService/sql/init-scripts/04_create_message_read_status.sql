CREATE TABLE message_read_status (
    message_id BIGINT NOT NULL REFERENCES messages(id) ON DELETE CASCADE,
    user_id BIGINT NOT NULL,
    read_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
    PRIMARY KEY (message_id, user_id)
);
