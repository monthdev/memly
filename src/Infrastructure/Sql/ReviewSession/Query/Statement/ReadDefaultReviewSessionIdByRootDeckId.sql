SELECT
  id::VARCHAR AS review_session_id
FROM
  review_sessions
WHERE
  root_deck_id_if_default = CAST(CAST(? AS VARCHAR) AS UUID);
