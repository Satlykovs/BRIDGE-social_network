CREATE TABLE messages (
    id BIGSERIAL PRIMARY KEY,
    conversation_id INT NOT NULL REFERENCES conversations(id) ON DELETE CASCADE,
    sender_id BIGINT NOT NULL,
    content TEXT,
    attachments JSONB DEFAULT '[]'::jsonb NOT NULL,
    reply_to_id BIGINT REFERENCES messages(id),
    sent_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
    edited_at TIMESTAMPTZ,
    deleted BOOLEAN DEFAULT FALSE NOT NULL,
    deletion_reason VARCHAR(20)
);
