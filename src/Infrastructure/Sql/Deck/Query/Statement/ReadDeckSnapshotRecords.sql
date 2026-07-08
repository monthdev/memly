WITH
  clock AS (
    SELECT
      EPOCH_MS(CAST(? AS BIGINT)) AS as_of_milliseconds_since_epoch
  )
SELECT
  decks.id::VARCHAR AS deck_id,
  decks.parent_deck_id::VARCHAR AS parent_deck_id,
  decks.name AS deck_name,
  EPOCH_MS(decks.created_at) AS created_at_milliseconds_since_epoch,
  EPOCH_MS(decks.last_updated_at) AS last_updated_at_milliseconds_since_epoch,
  CAST(
    COUNT(*) FILTER(
      WHERE
        cards.id IS NOT NULL
        AND (
          cards.scheduled_due_at IS NULL
          OR cards.scheduled_due_at <= clock.as_of_milliseconds_since_epoch
        )
    ) AS UINTEGER
  ) AS self_due_now_count,
  CAST(
    COUNT(*) FILTER(
      WHERE
        cards.id IS NOT NULL
        AND cards.scheduled_due_at > clock.as_of_milliseconds_since_epoch
        AND cards.scheduled_due_at < CAST(
          CAST(clock.as_of_milliseconds_since_epoch AS DATE) + INTERVAL 1 DAY AS TIMESTAMP
        )
    ) AS UINTEGER
  ) AS self_by_today_count,
  CAST(COUNT(cards.id) AS UINTEGER) AS self_total_count,
  decks.target_language_code
FROM
  decks
  CROSS JOIN clock
  LEFT JOIN cards ON cards.deck_id = decks.id
GROUP BY
  decks.id,
  decks.parent_deck_id,
  decks.name,
  decks.created_at,
  decks.last_updated_at,
  decks.target_language_code;
