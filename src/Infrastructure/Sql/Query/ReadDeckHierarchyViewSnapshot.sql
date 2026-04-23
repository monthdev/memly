WITH
  next_future_due AS (
    SELECT
      MIN(due_at) AS next_due_at
    FROM
      cards
    WHERE
      due_at > CURRENT_TIMESTAMP
  ),
  next_refresh AS (
    SELECT
      LEAST(
        next_due_at,
        CAST(CURRENT_DATE + INTERVAL 1 DAY AS TIMESTAMP)
      ) AS next_refresh_at
    FROM
      next_future_due
  )
SELECT
  id::VARCHAR AS id,
  COALESCE(parent_id::VARCHAR, '') AS parent_id,
  name,
  due_now_count,
  by_today_count,
  total_count,
  target_language_code,
  COALESCE(EPOCH_MS(next_refresh_at), -1) AS next_refresh_at_milliseconds_since_epoch
FROM
  deck_hierarchy_view
  CROSS JOIN next_refresh;
