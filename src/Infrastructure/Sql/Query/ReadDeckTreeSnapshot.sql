WITH
  clock AS (
    SELECT
      EPOCH_MS(CAST(? AS BIGINT)) AS as_of_at
  ),
  self_counts AS (
    SELECT
      decks.id AS deck_id,
      CAST(
        COUNT(*) FILTER(
          WHERE
            cards.id IS NOT NULL
            AND (
              cards.due_at IS NULL
              OR cards.due_at <= clock.as_of_at
            )
        ) AS UINTEGER
      ) AS self_due_now_count,
      CAST(
        COUNT(*) FILTER(
          WHERE
            cards.id IS NOT NULL
            AND cards.due_at > clock.as_of_at
            AND cards.due_at < CAST(
              CAST(clock.as_of_at AS DATE) + INTERVAL 1 DAY AS TIMESTAMP
            )
        ) AS UINTEGER
      ) AS self_by_today_count,
      CAST(COUNT(cards.id) AS UINTEGER) AS self_total_count
    FROM
      decks
      CROSS JOIN clock
      LEFT JOIN cards ON cards.deck_id = decks.id
    GROUP BY
      decks.id
  ),
  subtree_counts AS (
    SELECT
      deck_subtree_membership_view.subtree_root_deck_id AS deck_id,
      CAST(SUM(self_counts.self_due_now_count) AS UINTEGER) AS due_now_count,
      CAST(SUM(self_counts.self_by_today_count) AS UINTEGER) AS by_today_count,
      CAST(SUM(self_counts.self_total_count) AS UINTEGER) AS total_count
    FROM
      deck_subtree_membership_view
      INNER JOIN self_counts ON self_counts.deck_id = deck_subtree_membership_view.member_deck_id
    GROUP BY
      deck_subtree_membership_view.subtree_root_deck_id
  )
SELECT
  decks.id::VARCHAR AS id,
  COALESCE(decks.parent_deck_id::VARCHAR, '') AS parent_id,
  decks.name,
  subtree_counts.due_now_count,
  subtree_counts.by_today_count,
  subtree_counts.total_count,
  decks.target_language_code
FROM
  decks
  INNER JOIN subtree_counts ON subtree_counts.deck_id = decks.id;
