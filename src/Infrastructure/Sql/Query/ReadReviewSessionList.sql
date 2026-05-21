SELECT
  id::VARCHAR AS review_session_id,
  name,
  EPOCH_MS(last_opened_at) AS last_opened_at_milliseconds_since_epoch
FROM
  review_sessions;
