WITH RECURSIVE
  requested_move AS (
    SELECT
      CAST(CAST(? AS VARCHAR) AS UUID) AS deck_id,
      NULLIF(CAST(? AS VARCHAR), '')::UUID AS new_parent_deck_id
  ),
  validated_request AS (
    SELECT
      CASE
        WHEN EXISTS (
          SELECT
            1
          FROM
            decks
          WHERE
            id = requested_move.deck_id
        ) THEN requested_move.deck_id
        ELSE ERROR('Deck does not exist')
      END AS deck_id,
      CASE
        WHEN requested_move.new_parent_deck_id IS NULL THEN NULL
        WHEN EXISTS (
          SELECT
            1
          FROM
            decks
          WHERE
            id = requested_move.new_parent_deck_id
        ) THEN requested_move.new_parent_deck_id
        ELSE ERROR('Parent deck does not exist')
      END AS new_parent_deck_id
    FROM
      requested_move
  ),
  deck_subtree (id) AS (
    SELECT
      deck_id
    FROM
      validated_request
    UNION ALL
    SELECT
      child.id
    FROM
      decks AS child
      INNER JOIN deck_subtree ON child.parent_deck_id = deck_subtree.id
  ),
  validated_move AS (
    SELECT
      validated_request.deck_id,
      CASE
        WHEN validated_request.new_parent_deck_id IS NULL THEN NULL
        WHEN validated_request.new_parent_deck_id = validated_request.deck_id THEN ERROR('Deck cannot move into itself')
        WHEN EXISTS (
          SELECT
            1
          FROM
            deck_subtree
          WHERE
            id = validated_request.new_parent_deck_id
        ) THEN ERROR('Deck move would create a cycle')
        ELSE validated_request.new_parent_deck_id
      END AS new_parent_deck_id
    FROM
      validated_request
  )
UPDATE decks
SET
  parent_deck_id = validated_move.new_parent_deck_id,
  updated_at = CURRENT_TIMESTAMP
FROM
  validated_move
WHERE
  decks.id = validated_move.deck_id
RETURNING
  decks.id;
