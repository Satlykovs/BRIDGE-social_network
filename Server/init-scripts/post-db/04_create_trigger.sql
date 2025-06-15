CREATE OR REPLACE FUNCTION update_likes_count()
RETURNS TRIGGER AS $$
BEGIN
    IF (TG_OP = 'INSERT') THEN
        UPDATE posts 
        SET likes_count = likes_count + 1
        WHERE post_id = NEW.post_id;
    ELSIF (TG_OP = 'DELETE') THEN
        UPDATE posts 
        SET likes_count = likes_count - 1
        WHERE post_id = OLD.post_id;
    END IF;
    RETURN NULL;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_update_likes_count
AFTER INSERT OR DELETE ON likes
FOR EACH ROW EXECUTE FUNCTION update_likes_count();