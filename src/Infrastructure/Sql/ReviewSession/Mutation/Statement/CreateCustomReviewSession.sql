INSERT INTO
  review_sessions (custom_name, definition_key)
VALUES
  (?, ?)
RETURNING
  id::VARCHAR AS review_session_id;
