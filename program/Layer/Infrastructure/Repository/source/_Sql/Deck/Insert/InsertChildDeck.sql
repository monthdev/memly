WITH
  requested_parent AS (
    SELECT
      CAST(CAST(? AS VARCHAR) AS UUID) AS parent_deck_id
  ),
  child_creation_context AS (
    SELECT
      requested_parent.parent_deck_id,
      parent_deck.target_language_code,
      parent_deck.deck_settings_id
    FROM
      requested_parent
      INNER JOIN decks AS parent_deck ON parent_deck.id = requested_parent.parent_deck_id
  )
INSERT INTO
  decks (
    name,
    parent_deck_id,
    target_language_code,
    deck_settings_id
  )
SELECT
  ?,
  child_creation_context.parent_deck_id,
  child_creation_context.target_language_code,
  child_creation_context.deck_settings_id
FROM
  child_creation_context
RETURNING
  id::VARCHAR AS deck_id;
