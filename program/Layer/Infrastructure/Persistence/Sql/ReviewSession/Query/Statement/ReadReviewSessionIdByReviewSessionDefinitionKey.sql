SELECT
  id::VARCHAR AS review_session_id
FROM
  review_sessions
WHERE
  definition_key = ?;
