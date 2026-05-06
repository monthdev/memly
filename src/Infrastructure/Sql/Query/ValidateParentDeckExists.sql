WITH
  requested_parent_deck AS (
    SELECT
      CAST(CAST(? AS VARCHAR) AS UUID) AS parent_deck_id
  )
SELECT
  CASE
    WHEN EXISTS (
      SELECT
        1
      FROM
        decks
      WHERE
        id = requested_parent_deck.parent_deck_id
    ) THEN TRUE
    ELSE ERROR('Parent deck does not exist')
  END AS parent_deck_exists
FROM
  requested_parent_deck;
