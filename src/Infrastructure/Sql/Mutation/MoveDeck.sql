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
  move_language_context AS (
    SELECT
      validated_request.deck_id,
      validated_request.new_parent_deck_id,
      moved_deck.target_language_code AS deck_target_language_code,
      new_parent_deck.target_language_code AS parent_deck_target_language_code
    FROM
      validated_request
      INNER JOIN decks AS moved_deck ON moved_deck.id = validated_request.deck_id
      LEFT JOIN decks AS new_parent_deck ON new_parent_deck.id = validated_request.new_parent_deck_id
  ),
  validated_move AS (
    SELECT
      move_language_context.deck_id,
      CASE
        WHEN move_language_context.new_parent_deck_id IS NULL THEN NULL
        WHEN move_language_context.new_parent_deck_id = move_language_context.deck_id THEN ERROR('Deck cannot move into itself')
        WHEN EXISTS (
          SELECT
            1
          FROM
            deck_subtree
          WHERE
            id = move_language_context.new_parent_deck_id
        ) THEN ERROR('Deck move would create a cycle')
        WHEN move_language_context.parent_deck_target_language_code <> move_language_context.deck_target_language_code THEN ERROR(
          'Deck target language does not match parent deck'
        )
        ELSE move_language_context.new_parent_deck_id
      END AS new_parent_deck_id
    FROM
      move_language_context
  )
UPDATE decks
SET
  parent_deck_id = validated_move.new_parent_deck_id,
  updated_at = CURRENT_TIMESTAMP
FROM
  validated_move
WHERE
  decks.id = validated_move.deck_id;
