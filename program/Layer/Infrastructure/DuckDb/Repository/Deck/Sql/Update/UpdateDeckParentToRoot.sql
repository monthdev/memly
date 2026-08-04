UPDATE decks
SET
  parent_deck_id = NULL,
  last_updated_at = CURRENT_TIMESTAMP
WHERE
  id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  id::VARCHAR AS deck_id;
