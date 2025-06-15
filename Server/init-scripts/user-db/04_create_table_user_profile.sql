CREATE TABLE IF NOT EXISTS user_profile (
    user_id INT PRIMARY KEY REFERENCES users(id) ON DELETE CASCADE,
    email TEXT NOT NULL UNIQUE,
    first_name TEXT DEFAULT '',
    last_name TEXT DEFAULT '',
    username TEXT DEFAULT '',
    avatar_url TEXT DEFAULT 'https://storage.yandexcloud.net/bridge-bucket/avatars/default.jpg'
);