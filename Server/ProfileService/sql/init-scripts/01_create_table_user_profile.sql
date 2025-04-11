CREATE TABLE IF NOT EXISTS user_profile (
    id INT PRIMARY KEY,
    email TEXT NOT NULL UNIQUE,
    first_name TEXT DEFAULT '',
    last_name TEXT DEFAULT '',
    username TEXT DEFAULT '',
    avatar_url TEXT DEFAULT 'https://storage.yandexcloud.net/bridge-bucket/avatars/default.jpg',
    is_first_login BOOLEAN DEFAULT TRUE
);