WITH
  requested_parent AS (
    SELECT
      NULLIF(CAST(? AS VARCHAR), '')::UUID AS parent_deck_id
  ),
  validated_parent AS (
    SELECT
      CASE
        WHEN requested_parent.parent_deck_id IS NULL THEN ERROR('Parent deck does not exist')
        WHEN EXISTS (
          SELECT
            1
          FROM
            decks
          WHERE
            id = requested_parent.parent_deck_id
        ) THEN requested_parent.parent_deck_id
        ELSE ERROR('Parent deck does not exist')
      END AS parent_deck_id
    FROM
      requested_parent
  ),
  child_creation_context AS (
    SELECT
      validated_parent.parent_deck_id,
      parent_deck.target_language_code,
      parent_deck.deck_settings_id
    FROM
      validated_parent
      INNER JOIN decks AS parent_deck ON parent_deck.id = validated_parent.parent_deck_id
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
  child_creation_context;
