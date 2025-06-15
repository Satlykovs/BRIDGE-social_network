CREATE UNIQUE INDEX unique_sent_requests 
ON friend_requests (sender_id, receiver_id) 
WHERE status = 'sent';