SELECT sender_id
FROM friend_requests
WHERE receiver_id = $1 AND status = 'sent';