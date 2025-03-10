SELECT 
  CASE 
    WHEN sender_id = $1 THEN receiver_id 
    ELSE sender_id 
  END AS friend_id
FROM friend_requests 
WHERE 
  (sender_id = $1 OR receiver_id = $1) 
  AND status = 'accepted';