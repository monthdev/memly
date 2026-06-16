SELECT
  review_sessions.id::VARCHAR AS review_session_id,
  COALESCE(review_sessions.custom_name, decks.name) AS name,
  EPOCH_MS(review_sessions.created_at) AS created_at_milliseconds_since_epoch,
  EPOCH_MS(review_sessions.last_updated_at) AS last_updated_at_milliseconds_since_epoch,
  EPOCH_MS(review_sessions.last_card_review_at) AS last_card_review_at_milliseconds_since_epoch
FROM
  review_sessions
  LEFT JOIN decks ON decks.id = review_sessions.root_deck_id_if_default;
