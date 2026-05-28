DELETE FROM review_sessions
WHERE
  id = ?
RETURNING
  id;
