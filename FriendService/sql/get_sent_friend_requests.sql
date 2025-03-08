SELECT receiver_id
FROM friend_requests
WHERE sender_id = $1 AND status = 'sent';