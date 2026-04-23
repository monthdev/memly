BEGIN TRANSACTION;

CREATE MACRO IF NOT EXISTS deck_name_length_is_valid (deck_name) AS LENGTH(deck_name) > 0
AND LENGTH(deck_name) <= 40;

CREATE MACRO IF NOT EXISTS settings_name_length_is_valid (settings_name) AS LENGTH(settings_name) > 0
AND LENGTH(settings_name) <= 40;

CREATE MACRO IF NOT EXISTS card_front_text_length_is_valid (card_front_text) AS LENGTH(card_front_text) > 0
AND LENGTH(card_front_text) <= 200;

CREATE MACRO IF NOT EXISTS card_back_text_length_is_valid (card_back_text) AS LENGTH(card_back_text) > 0
AND LENGTH(card_back_text) <= 500;

CREATE MACRO IF NOT EXISTS user_flag_is_valid (user_flag) AS user_flag BETWEEN 0 AND 7;

CREATE MACRO IF NOT EXISTS target_language_code_is_valid (target_language_code) AS target_language_code BETWEEN 0 AND 66;

CREATE MACRO IF NOT EXISTS desired_retention_is_valid (desired_retention) AS desired_retention > 0.7
AND desired_retention < 1.0;

CREATE MACRO IF NOT EXISTS maximum_interval_days_is_valid (maximum_interval_days) AS maximum_interval_days > 0;

CREATE MACRO IF NOT EXISTS fsrs_difficulty_before_is_valid (fsrs_difficulty_before) AS fsrs_difficulty_before = 0.0
OR (
  fsrs_difficulty_before >= 1.0
  AND fsrs_difficulty_before <= 10.0
);

CREATE MACRO IF NOT EXISTS fsrs_difficulty_after_is_valid (fsrs_difficulty_after) AS fsrs_difficulty_after >= 1.0
AND fsrs_difficulty_after <= 10.0;

CREATE MACRO IF NOT EXISTS fsrs_stability_before_is_valid (fsrs_stability_before) AS fsrs_stability_before >= 0.0;

CREATE MACRO IF NOT EXISTS fsrs_stability_after_is_valid (fsrs_stability_after) AS fsrs_stability_after > 0.0;

CREATE MACRO IF NOT EXISTS review_rating_is_valid (review_rating) AS review_rating BETWEEN 1 AND 4;

CREATE MACRO IF NOT EXISTS elapsed_interval_seconds_is_valid (elapsed_interval_seconds) AS elapsed_interval_seconds IS NULL
OR elapsed_interval_seconds > 0.0;

CREATE MACRO IF NOT EXISTS scheduled_interval_seconds_is_valid (scheduled_interval_seconds) AS scheduled_interval_seconds IS NULL
OR scheduled_interval_seconds > 0.0;

CREATE MACRO IF NOT EXISTS answer_time_milliseconds_is_valid (answer_time_milliseconds) AS answer_time_milliseconds > 0;

CREATE MACRO IF NOT EXISTS updated_at_time_is_valid (created_at, updated_at) AS updated_at IS NULL
OR updated_at > created_at;

CREATE MACRO IF NOT EXISTS deck_parent_is_valid (deck_id, parent_deck_id) AS parent_deck_id IS NULL
OR parent_deck_id <> deck_id;

CREATE MACRO IF NOT EXISTS due_time_after_last_review_time_is_valid (last_reviewed_at, due_at) AS (
  last_reviewed_at IS NULL
  AND due_at IS NULL
)
OR due_at > last_reviewed_at;

CREATE TYPE scheduler_type AS ENUM('fsrs');

CREATE MACRO IF NOT EXISTS deck_settings_scheduler_combo_is_valid (scheduler_type_value, fsrs_settings_id_value) AS (
  scheduler_type_value = 'fsrs'
  AND fsrs_settings_id_value IS NOT NULL
);

CREATE MACRO IF NOT EXISTS card_review_scheduler_combo_is_valid (scheduler_type_value, fsrs_scheduler_id_value) AS (
  scheduler_type_value = 'fsrs'
  AND fsrs_scheduler_id_value IS NOT NULL
);

CREATE TABLE IF NOT EXISTS fsrs_schedulers (
  id UUID PRIMARY KEY DEFAULT UUIDV7 (),
  name VARCHAR NOT NULL UNIQUE CHECK (settings_name_length_is_valid (name)),
  algorithm_version UINTEGER NOT NULL,
  is_version_default BOOLEAN NOT NULL DEFAULT FALSE,
  optimization_interval UINTEGER NOT NULL DEFAULT 512,
  total_optimizations UINTEGER NOT NULL DEFAULT 0,
  maximum_interval_days UINTEGER NOT NULL CHECK (
    maximum_interval_days_is_valid (maximum_interval_days)
  ),
  parameters_json JSON NOT NULL,
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP CHECK (updated_at_time_is_valid (created_at, updated_at))
);

CREATE UNIQUE INDEX IF NOT EXISTS fsrs_schedulers_ensure_single_version_default_idx ON fsrs_schedulers (
  (
    CASE
      WHEN is_version_default THEN algorithm_version
      ELSE NULL
    END
  )
);

CREATE TABLE IF NOT EXISTS fsrs_scheduler_parameter_snapshots (
  id UUID PRIMARY KEY DEFAULT UUIDV7 (),
  fsrs_scheduler_id UUID NOT NULL REFERENCES fsrs_schedulers (id),
  parameters_json JSON NOT NULL,
  recorded_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  UNIQUE (id, fsrs_scheduler_id)
);

CREATE TABLE IF NOT EXISTS fsrs_settings (
  id UUID PRIMARY KEY DEFAULT UUIDV7 (),
  name VARCHAR NOT NULL UNIQUE CHECK (settings_name_length_is_valid (name)),
  is_default BOOLEAN NOT NULL DEFAULT FALSE,
  fsrs_scheduler_id UUID NOT NULL REFERENCES fsrs_schedulers (id),
  desired_retention DOUBLE NOT NULL CHECK (desired_retention_is_valid (desired_retention)),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP CHECK (updated_at_time_is_valid (created_at, updated_at))
);

CREATE UNIQUE INDEX IF NOT EXISTS fsrs_settings_ensure_single_default_idx ON fsrs_settings (
  (
    CASE
      WHEN is_default THEN 1
      ELSE NULL
    END
  )
);

CREATE TABLE IF NOT EXISTS deck_settings (
  id UUID PRIMARY KEY DEFAULT UUIDV7 (),
  name VARCHAR NOT NULL UNIQUE CHECK (settings_name_length_is_valid (name)),
  is_default BOOLEAN NOT NULL DEFAULT FALSE,
  fsrs_settings_id UUID REFERENCES fsrs_settings (id),
  scheduler_type scheduler_type NOT NULL DEFAULT 'fsrs' CHECK (
    deck_settings_scheduler_combo_is_valid (scheduler_type, fsrs_settings_id)
  ),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP CHECK (updated_at_time_is_valid (created_at, updated_at))
);

CREATE UNIQUE INDEX IF NOT EXISTS deck_settings_ensure_single_default_idx ON deck_settings (
  (
    CASE
      WHEN is_default THEN 1
      ELSE NULL
    END
  )
);

-- NOTE: There is nothing enforcing non-existent `parent_deck_id`s in the schema due to limitations of DuckDB.
CREATE TABLE IF NOT EXISTS decks (
  id UUID PRIMARY KEY DEFAULT UUIDV7 (),
  parent_deck_id UUID CHECK (deck_parent_is_valid (id, parent_deck_id)),
  name VARCHAR NOT NULL CHECK (deck_name_length_is_valid (name)),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP CHECK (updated_at_time_is_valid (created_at, updated_at)),
  target_language_code UTINYINT NOT NULL CHECK (
    target_language_code_is_valid (target_language_code)
  ),
  deck_settings_id UUID NOT NULL REFERENCES deck_settings (id)
);

CREATE UNIQUE INDEX IF NOT EXISTS decks_parent_deck_name_idx ON decks (
  (
    COALESCE(
      parent_deck_id,
      '00000000-0000-0000-0000-000000000000'::UUID
    )
  ),
  name
);

CREATE INDEX IF NOT EXISTS decks_parent_deck_id_idx ON decks (parent_deck_id);

CREATE TABLE IF NOT EXISTS cards (
  id UUID PRIMARY KEY DEFAULT UUIDV7 (),
  deck_id UUID NOT NULL REFERENCES decks (id),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP CHECK (updated_at_time_is_valid (created_at, updated_at)),
  last_reviewed_at TIMESTAMP,
  due_at TIMESTAMP CHECK (
    due_time_after_last_review_time_is_valid (last_reviewed_at, due_at)
  ),
  difficulty DOUBLE NOT NULL DEFAULT 0.0 CHECK (fsrs_difficulty_before_is_valid (difficulty)),
  stability DOUBLE NOT NULL DEFAULT 0.0 CHECK (fsrs_stability_before_is_valid (stability)),
  front_text VARCHAR NOT NULL CHECK (card_front_text_length_is_valid (front_text)),
  front_normalized_text VARCHAR GENERATED ALWAYS AS (LOWER(STRIP_ACCENTS(NFC_NORMALIZE(front_text)))),
  back_text VARCHAR NOT NULL CHECK (card_back_text_length_is_valid (back_text)),
  back_normalized_text VARCHAR GENERATED ALWAYS AS (LOWER(STRIP_ACCENTS(NFC_NORMALIZE(back_text)))),
  audio_path VARCHAR,
  user_flag UTINYINT NOT NULL DEFAULT 0 CHECK (user_flag_is_valid (user_flag)),
  UNIQUE (deck_id, front_text)
);

CREATE TABLE IF NOT EXISTS card_reviews (
  id UUID PRIMARY KEY DEFAULT UUIDV7 (),
  card_id UUID NOT NULL REFERENCES cards (id),
  deck_id UUID NOT NULL REFERENCES decks (id),
  scheduler_type scheduler_type NOT NULL CHECK (
    card_review_scheduler_combo_is_valid (scheduler_type, fsrs_scheduler_id)
  ),
  desired_retention_snapshot DOUBLE NOT NULL CHECK (
    desired_retention_is_valid (desired_retention_snapshot)
  ),
  maximum_interval_days_snapshot UINTEGER NOT NULL CHECK (
    maximum_interval_days_is_valid (maximum_interval_days_snapshot)
  ),
  fsrs_scheduler_id UUID REFERENCES fsrs_schedulers (id),
  fsrs_scheduler_parameter_snapshot_id UUID NOT NULL REFERENCES fsrs_scheduler_parameter_snapshots (id),
  FOREIGN KEY (
    fsrs_scheduler_parameter_snapshot_id,
    fsrs_scheduler_id
  ) REFERENCES fsrs_scheduler_parameter_snapshots (id, fsrs_scheduler_id),
  reviewed_at TIMESTAMP NOT NULL,
  rating UTINYINT NOT NULL CHECK (review_rating_is_valid (rating)),
  elapsed_interval_seconds DOUBLE CHECK (
    elapsed_interval_seconds_is_valid (elapsed_interval_seconds)
  ),
  scheduled_interval_seconds DOUBLE CHECK (
    scheduled_interval_seconds_is_valid (scheduled_interval_seconds)
  ),
  answer_time_milliseconds UINTEGER NOT NULL CHECK (
    answer_time_milliseconds_is_valid (answer_time_milliseconds)
  ),
  stability_before DOUBLE NOT NULL CHECK (fsrs_stability_before_is_valid (stability_before)),
  stability_after DOUBLE NOT NULL CHECK (fsrs_stability_after_is_valid (stability_after)),
  difficulty_before DOUBLE NOT NULL CHECK (
    fsrs_difficulty_before_is_valid (difficulty_before)
  ),
  difficulty_after DOUBLE NOT NULL CHECK (fsrs_difficulty_after_is_valid (difficulty_after))
);

CREATE VIEW IF NOT EXISTS deck_hierarchy_view AS
WITH RECURSIVE
  own_counts AS (
    SELECT
      decks.id,
      CAST(
        COUNT(*) FILTER(
          WHERE
            cards.id IS NOT NULL
            AND (
              cards.due_at IS NULL
              OR cards.due_at <= CURRENT_TIMESTAMP
            )
        ) AS UINTEGER
      ) AS own_due_now_count,
      CAST(
        COUNT(*) FILTER(
          WHERE
            cards.id IS NOT NULL
            AND cards.due_at > CURRENT_TIMESTAMP
            AND cards.due_at < CAST(CURRENT_DATE + INTERVAL 1 DAY AS TIMESTAMP)
        ) AS UINTEGER
      ) AS own_by_today_count,
      CAST(COUNT(cards.id) AS UINTEGER) AS own_total_count
    FROM
      decks
      LEFT JOIN cards ON decks.id = cards.deck_id
    GROUP BY
      decks.id
  ),
  deck_descendants (ancestor_id, descendant_id) AS (
    SELECT
      id,
      id
    FROM
      decks
    UNION ALL
    SELECT
      deck_descendants.ancestor_id,
      child.id
    FROM
      deck_descendants
      INNER JOIN decks AS child ON child.parent_deck_id = deck_descendants.descendant_id
  ),
  aggregate_counts AS (
    SELECT
      deck_descendants.ancestor_id AS deck_id,
      CAST(SUM(own_counts.own_due_now_count) AS UINTEGER) AS due_now_count,
      CAST(SUM(own_counts.own_by_today_count) AS UINTEGER) AS by_today_count,
      CAST(SUM(own_counts.own_total_count) AS UINTEGER) AS total_count
    FROM
      deck_descendants
      INNER JOIN own_counts ON own_counts.id = deck_descendants.descendant_id
    GROUP BY
      deck_descendants.ancestor_id
  )
SELECT
  decks.id,
  decks.parent_deck_id AS parent_id,
  decks.name,
  aggregate_counts.due_now_count,
  aggregate_counts.by_today_count,
  aggregate_counts.total_count,
  decks.target_language_code
FROM
  decks
  INNER JOIN aggregate_counts ON aggregate_counts.deck_id = decks.id;

COMMIT;
