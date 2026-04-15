WITH
  next_future_due AS (
    SELECT
      MIN(due_at) AS next_due_at
    FROM
      cards
    WHERE
      due_at > CURRENT_TIMESTAMP
  )
SELECT
  CASE
    WHEN next_due_at IS NULL THEN -1
    ELSE GREATEST(
      0,
      DATE_DIFF(
        'millisecond',
        CURRENT_TIMESTAMP,
        LEAST(
          next_due_at,
          CAST(CURRENT_DATE + INTERVAL 1 DAY AS TIMESTAMP)
        )
      )
    )
  END AS next_refresh_delay_milliseconds
FROM
  next_future_due;

-- NOTE: The `GREATEST` guard is technically unnecessary as the `CURRENT_TIMESTAMP` value is taken once at the start of the current transaction.
