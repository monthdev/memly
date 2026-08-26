INSERT INTO
  review_sessions (root_deck_id_if_default, definition_key)
SELECT
  decks.id,
  ?
FROM
  decks
WHERE
  decks.id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  id::VARCHAR AS review_session_id;
