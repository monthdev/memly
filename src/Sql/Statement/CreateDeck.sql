INSERT INTO
  decks (
    name,
    target_language_code,
    deck_settings_id
  )
VALUES
  (
    ?,
    ?,
    (
      SELECT
        id
      FROM
        deck_settings
      WHERE
        is_default = true
      LIMIT
        1
    )
  );
