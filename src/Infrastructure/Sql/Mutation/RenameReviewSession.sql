UPDATE review_sessions
SET
  custom_name = ?,
  last_updated_at = CURRENT_TIMESTAMP
WHERE
  id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  id;
