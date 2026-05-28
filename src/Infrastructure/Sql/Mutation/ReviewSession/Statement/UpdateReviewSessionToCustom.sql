UPDATE review_sessions
SET
  root_deck_id_if_default = NULL,
  custom_name = COALESCE(
    custom_name,
    (
      SELECT
        decks.name
      FROM
        decks
      WHERE
        decks.id = review_sessions.root_deck_id_if_default
    )
  ),
  definition_key = ?,
  last_updated_at = CURRENT_TIMESTAMP
WHERE
  review_sessions.id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  review_sessions.id::VARCHAR AS review_session_id;
