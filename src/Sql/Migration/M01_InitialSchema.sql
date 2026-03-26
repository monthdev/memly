BEGIN TRANSACTION;

CREATE MACRO IF NOT EXISTS deck_name_is_valid (deck_name) AS length(deck_name) > 0
AND length(deck_name) <= 40;

CREATE MACRO IF NOT EXISTS card_front_text_is_valid (card_front_text) AS length(card_front_text) > 0
AND length(card_front_text) <= 200;

CREATE MACRO IF NOT EXISTS card_back_text_is_valid (card_back_text) AS length(card_back_text) > 0
AND length(card_back_text) <= 500;

CREATE MACRO IF NOT EXISTS user_flag_is_valid (user_flag) AS user_flag BETWEEN 0 AND 7;

CREATE MACRO IF NOT EXISTS target_language_code_is_valid (target_language_code) AS target_language_code BETWEEN 0 AND 66;

CREATE MACRO IF NOT EXISTS desired_retention_is_valid (desired_retention) AS desired_retention > 0.0
AND desired_retention < 1.0;

CREATE MACRO IF NOT EXISTS maximum_interval_days_is_valid (maximum_interval_days) AS maximum_interval_days > 0;

CREATE MACRO IF NOT EXISTS fsrs_difficulty_is_valid (fsrs_difficulty) AS fsrs_difficulty >= 1.0
AND fsrs_difficulty <= 10.0;

CREATE MACRO IF NOT EXISTS fsrs_stability_is_valid (fsrs_stability) AS fsrs_stability > 0.0;

CREATE MACRO IF NOT EXISTS review_rating_is_valid (review_rating) AS review_rating BETWEEN 1 AND 4;

CREATE MACRO IF NOT EXISTS elapsed_seconds_is_valid (elapsed_seconds) AS elapsed_seconds >= 0.0;

CREATE MACRO IF NOT EXISTS scheduled_seconds_is_valid (scheduled_seconds) AS scheduled_seconds IS NULL
OR scheduled_seconds >= 0.0;

CREATE MACRO IF NOT EXISTS answer_time_milliseconds_is_valid (answer_time_milliseconds) AS answer_time_milliseconds IS NULL
OR answer_time_milliseconds >= 0;

CREATE MACRO IF NOT EXISTS updated_at_is_valid (created_at, updated_at) AS updated_at IS NULL
OR updated_at >= created_at;

CREATE MACRO IF NOT EXISTS due_after_last_review_is_valid (last_reviewed_at, due_at) AS last_reviewed_at IS NULL
OR due_at IS NULL
OR due_at >= last_reviewed_at;

CREATE TABLE IF NOT EXISTS fsrs_configurations (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  name VARCHAR NOT NULL UNIQUE,
  algorithm_version VARCHAR NOT NULL,
  desired_retention DOUBLE NOT NULL CHECK (desired_retention_is_valid (desired_retention)),
  maximum_interval_days UINTEGER NOT NULL CHECK (
    maximum_interval_days_is_valid (maximum_interval_days)
  ),
  parameters_json JSON NOT NULL,
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP,
  CHECK (updated_at_is_valid (created_at, updated_at))
);

CREATE TABLE IF NOT EXISTS decks (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  name VARCHAR NOT NULL UNIQUE CHECK (deck_name_is_valid (name)),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP,
  target_language_code UTINYINT NOT NULL CHECK (
    target_language_code_is_valid (target_language_code)
  ),
  fsrs_configuration_id UUID NOT NULL REFERENCES fsrs_configurations (id),
  CHECK (updated_at_is_valid (created_at, updated_at))
);

CREATE TABLE IF NOT EXISTS cards (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  deck_id UUID NOT NULL REFERENCES decks (id),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP,
  last_reviewed_at TIMESTAMP,
  due_at TIMESTAMP,
  difficulty DOUBLE NOT NULL CHECK (fsrs_difficulty_is_valid (difficulty)),
  stability DOUBLE NOT NULL CHECK (fsrs_stability_is_valid (stability)),
  front_text VARCHAR NOT NULL CHECK (card_front_text_is_valid (front_text)),
  front_normalized_text VARCHAR GENERATED ALWAYS AS (lower(strip_accents(nfc_normalize(front_text)))),
  back_text VARCHAR NOT NULL CHECK (card_back_text_is_valid (back_text)),
  back_normalized_text VARCHAR GENERATED ALWAYS AS (lower(strip_accents(nfc_normalize(back_text)))),
  audio_path VARCHAR,
  user_flag UTINYINT NOT NULL DEFAULT 0 CHECK (user_flag_is_valid (user_flag)),
  UNIQUE (deck_id, front_text),
  CHECK (updated_at_is_valid (created_at, updated_at)),
  CHECK (
    due_after_last_review_is_valid (last_reviewed_at, due_at)
  )
);

CREATE TABLE IF NOT EXISTS card_reviews (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  card_id UUID NOT NULL REFERENCES cards (id),
  reviewed_at TIMESTAMP NOT NULL,
  rating UTINYINT NOT NULL CHECK (review_rating_is_valid (rating)),
  elapsed_seconds DOUBLE NOT NULL CHECK (elapsed_seconds_is_valid (elapsed_seconds)),
  scheduled_seconds DOUBLE CHECK (scheduled_seconds_is_valid (scheduled_seconds)),
  answer_time_milliseconds UINTEGER CHECK (
    answer_time_milliseconds_is_valid (answer_time_milliseconds)
  ),
  stability_before DOUBLE CHECK (fsrs_stability_is_valid (stability_before)),
  stability_after DOUBLE CHECK (fsrs_stability_is_valid (stability_after)),
  difficulty_before DOUBLE CHECK (fsrs_difficulty_is_valid (difficulty_before)),
  difficulty_after DOUBLE CHECK (fsrs_difficulty_is_valid (difficulty_after))
);

COMMIT;
