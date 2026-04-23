WITH
  default_deck_settings AS (
    SELECT
      id AS deck_settings_id
    FROM
      deck_settings
    WHERE
      is_default = TRUE
    LIMIT
      1
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
  NULL,
  ?,
  default_deck_settings.deck_settings_id
FROM
  default_deck_settings;
