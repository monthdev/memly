UPDATE decks
SET
  name = ?,
  updated_at = CURRENT_TIMESTAMP
WHERE
  id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  id;
