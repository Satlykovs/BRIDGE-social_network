INSERT INTO reactions (post_id, user_id, reaction)
VALUES ($1, $2, 'like')
ON CONFLICT (post_id, user_id)
DO UPDATE SET reaction = EXCLUDED.reaction;
