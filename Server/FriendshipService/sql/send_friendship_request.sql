INSERT INTO friend_requests (sender_id, receiver_id, status)
VALUES ($1, $2, 'sent')
ON CONFLICT (sender_id, receiver_id) WHERE status = 'sent' 
DO NOTHING;