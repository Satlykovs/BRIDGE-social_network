UPDATE friend_requests
SET status = 'accepted'
WHERE sender_id = $2 AND receiver_id = $1