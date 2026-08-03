DELETE FROM review_sessions
WHERE
  id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  id::VARCHAR AS review_session_id;
