DELETE FROM friend_requests
WHERE (sender_id = $1 AND receiver_id = $2)
    AND status = 'sent';