UPDATE review_sessions
SET
  root_deck_id_if_default = CAST(CAST(? AS VARCHAR) AS UUID),
  custom_name = NULL,
  definition_key = ?,
  last_updated_at = CURRENT_TIMESTAMP
WHERE
  review_sessions.id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  review_sessions.id::VARCHAR AS review_session_id;
