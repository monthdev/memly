SELECT
  id::VARCHAR AS review_session_id,
  name,
  EPOCH_MS(created_at) AS created_at_milliseconds_since_epoch,
  EPOCH_MS(last_card_review_at) AS last_card_review_at_milliseconds_since_epoch
FROM
  review_sessions
ORDER BY
  COALESCE(last_card_review_at, created_at) DESC;
