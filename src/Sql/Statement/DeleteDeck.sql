BEGIN TRANSACTION;

DELETE FROM card_reviews
WHERE
  deck_id = ?;

DELETE FROM cards
WHERE
  deck_id = ?;

DELETE FROM decks
WHERE
  id = ?;

COMMIT;
