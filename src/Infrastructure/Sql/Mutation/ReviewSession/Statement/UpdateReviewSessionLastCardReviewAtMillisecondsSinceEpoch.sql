UPDATE review_sessions
SET
  last_card_review_at = CURRENT_TIMESTAMP
WHERE
  id = ?
RETURNING
  id;
