DELETE FROM custom_review_session_deck_selections
WHERE
  review_session_id = CAST(CAST(? AS VARCHAR) AS UUID)
RETURNING
  review_session_id::VARCHAR AS review_session_id,
  deck_id::VARCHAR AS deck_id;
