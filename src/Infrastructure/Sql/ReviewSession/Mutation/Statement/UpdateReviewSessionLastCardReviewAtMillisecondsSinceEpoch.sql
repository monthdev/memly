UPDATE review_sessions
SET
  last_card_review_at = CURRENT_TIMESTAMP
WHERE
  id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  id::VARCHAR AS review_session_id;
