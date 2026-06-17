DELETE FROM decks
WHERE
  id IN (
    WITH RECURSIVE
      deck_subtree (id) AS (
        SELECT
          CAST(CAST(? AS VARCHAR) AS UUID)
        UNION ALL
        SELECT
          child.id
        FROM
          decks AS child
          INNER JOIN deck_subtree ON child.parent_deck_id = deck_subtree.id
      )
    SELECT
      id
    FROM
      deck_subtree
  );
