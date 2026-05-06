WITH
  requested_deck AS (
    SELECT
      CAST(CAST(? AS VARCHAR) AS UUID) AS deck_id
  )
SELECT
  CASE
    WHEN EXISTS (
      SELECT
        1
      FROM
        decks
      WHERE
        id = requested_deck.deck_id
    ) THEN TRUE
    ELSE ERROR('Deck does not exist')
  END AS deck_exists
FROM
  requested_deck;
