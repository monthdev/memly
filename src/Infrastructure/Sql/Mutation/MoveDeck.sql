WITH RECURSIVE
  requested_move AS (
    SELECT
      CAST(CAST(? AS VARCHAR) AS UUID) AS deck_id,
      CAST(CAST(? AS VARCHAR) AS UUID) AS new_parent_deck_id
  ),
  deck_subtree (id) AS (
    SELECT
      deck_id
    FROM
      requested_move
    UNION ALL
    SELECT
      child.id
    FROM
      decks AS child
      INNER JOIN deck_subtree ON child.parent_deck_id = deck_subtree.id
  ),
  move_context AS (
    SELECT
      requested_move.deck_id AS moved_deck_id,
      requested_move.new_parent_deck_id,
      moved_deck.target_language_code AS moved_deck_target_language_code,
      new_parent_deck.id AS existing_new_parent_deck_id,
      new_parent_deck.target_language_code AS new_parent_deck_target_language_code
    FROM
      requested_move
      INNER JOIN decks AS moved_deck ON moved_deck.id = requested_move.deck_id
      LEFT JOIN decks AS new_parent_deck ON new_parent_deck.id = requested_move.new_parent_deck_id
  ),
  validated_move AS (
    SELECT
      move_context.moved_deck_id,
      CASE
        WHEN move_context.new_parent_deck_id IS NULL THEN NULL
        WHEN move_context.new_parent_deck_id = move_context.moved_deck_id THEN ERROR('Deck cannot move into itself')
        WHEN EXISTS (
          SELECT
            1
          FROM
            deck_subtree
          WHERE
            id = move_context.new_parent_deck_id
        ) THEN ERROR('Deck move would create a cycle')
        WHEN move_context.new_parent_deck_target_language_code <> move_context.moved_deck_target_language_code THEN ERROR('Deck target language does not match parent deck')
        ELSE move_context.new_parent_deck_id
      END AS new_parent_deck_id
    FROM
      move_context
    WHERE
      move_context.new_parent_deck_id IS NULL
      OR move_context.existing_new_parent_deck_id IS NOT NULL
  )
UPDATE decks
SET
  parent_deck_id = validated_move.new_parent_deck_id,
  updated_at = CURRENT_TIMESTAMP
FROM
  validated_move
WHERE
  decks.id = validated_move.moved_deck_id
RETURNING
  id;
