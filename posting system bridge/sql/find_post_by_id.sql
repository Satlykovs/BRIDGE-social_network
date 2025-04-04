SELECT
  p.post_id,
  p.author_id AS user_id,
  p.post_text AS content,
  p.created_at,
  (SELECT COUNT(*) FROM reactions WHERE post_id = p.post_id AND reaction = 'like') AS likes,
  (SELECT COUNT(*) FROM reactions WHERE post_id = p.post_id AND reaction = 'dislike') AS dislikes
FROM posts p
WHERE p.post_id = $1;
