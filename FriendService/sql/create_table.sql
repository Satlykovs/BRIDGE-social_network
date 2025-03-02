CREATE TYPE status_enam AS ENUM ('accepted','sent');

CREATE TABLE IF NOT EXISTS friend_requests(
    friends_request_id SERIAL PRIMARY KEY,
    sender_id INT NOT NULL,
    receiver_id INT NOT NULL,
    status status_enam NOT NULL
)