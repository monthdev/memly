WITH
  clock AS (
    SELECT
      EPOCH_MS(CAST(? AS BIGINT)) AS as_of_at
  ),
  next_future_due AS (
    SELECT
      MIN(cards.due_at) AS next_due_at
    FROM
      cards
      CROSS JOIN clock
    WHERE
      cards.due_at > clock.as_of_at
  ),
  next_refresh AS (
    SELECT
      LEAST(
        next_due_at,
        CAST(
          CAST(clock.as_of_at AS DATE) + INTERVAL 1 DAY AS TIMESTAMP
        )
      ) AS next_refresh_at
    FROM
      next_future_due
      CROSS JOIN clock
  )
SELECT
  EPOCH_MS(next_refresh_at) AS next_library_refresh_at_milliseconds_since_epoch
FROM
  next_refresh;
