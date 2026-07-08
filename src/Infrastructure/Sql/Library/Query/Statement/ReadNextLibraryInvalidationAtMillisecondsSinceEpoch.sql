WITH
  clock AS (
    SELECT
      EPOCH_MS(CAST(? AS BIGINT)) AS as_of_at
  ),
  next_future_scheduled_due AS (
    SELECT
      MIN(cards.scheduled_due_at) AS next_scheduled_due_at
    FROM
      cards
      CROSS JOIN clock
    WHERE
      cards.scheduled_due_at > clock.as_of_at
  ),
  next_invalidation AS (
    SELECT
      LEAST(
        next_scheduled_due_at,
        CAST(
          CAST(clock.as_of_at AS DATE) + INTERVAL 1 DAY AS TIMESTAMP
        )
      ) AS next_invalidation_at
    FROM
      next_future_scheduled_due
      CROSS JOIN clock
  )
SELECT
  EPOCH_MS(next_invalidation_at) AS next_library_invalidation_at_milliseconds_since_epoch
FROM
  next_invalidation;
