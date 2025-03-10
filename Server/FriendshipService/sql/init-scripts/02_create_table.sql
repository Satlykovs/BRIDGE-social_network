
CREATE TABLE IF NOT EXISTS friend_requests(
    friends_request_id SERIAL PRIMARY KEY,
    sender_id INT NOT NULL,
    receiver_id INT NOT NULL,
    status status_enum NOT NULL
)