INSERT INTO
  custom_review_session_deck_selections (review_session_id, deck_id, selection_type)
VALUES
  (
    CAST(CAST(? AS VARCHAR) AS UUID),
    CAST(CAST(? AS VARCHAR) AS UUID),
    ?
  )
RETURNING
  review_session_id::VARCHAR AS review_session_id,
  deck_id::VARCHAR AS deck_id;
