CREATE TABLE uploads (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    user_id BIGINT NOT NULL,
    conversation_id INT REFERENCES conversations(id) ON DELETE SET NULL,
    original_name VARCHAR(255) NOT NULL,
    s3_key VARCHAR(512) NOT NULL,
    preview_s3_key VARCHAR(512),
    mime_type VARCHAR(100) NOT NULL,
    file_size BIGINT NOT NULL,
    width INT,
    height INT,
    duration INT,
    status VARCHAR(20) NOT NULL DEFAULT 'pending',
    created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
    completed_at TIMESTAMPTZ
);
