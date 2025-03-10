UPDATE friend_requests
SET status = $3
WHERE sender_id = $2 AND receiver_id = $1