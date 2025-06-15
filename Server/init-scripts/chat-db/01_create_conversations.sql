CREATE TABLE conversations (
    id SERIAL PRIMARY KEY,
    type VARCHAR(10) NOT NULL,
    title VARCHAR(100),
    last_message_id BIGINT,
    created_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
    updated_at TIMESTAMPTZ DEFAULT NOW() NOT NULL,
    deleted BOOLEAN DEFAULT FALSE NOT NULL 
);

