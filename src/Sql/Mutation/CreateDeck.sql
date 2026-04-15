WITH
  requested_parent AS (
    SELECT
      NULLIF(CAST(? AS VARCHAR), '')::UUID AS parent_deck_id
  ),
  validated_parent AS (
    SELECT
      CASE
        WHEN requested_parent.parent_deck_id IS NULL THEN NULL
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
  validated_parent.parent_deck_id,
  ?,
  (
    SELECT
      id
    FROM
      deck_settings
    WHERE
      is_default = TRUE
    LIMIT
      1
  )
FROM
  validated_parent;
