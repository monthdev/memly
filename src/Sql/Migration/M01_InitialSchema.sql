BEGIN TRANSACTION;

CREATE MACRO IF NOT EXISTS deck_name_is_valid(name) AS
  length(name) > 0
  AND length(name) <= 40;

CREATE MACRO IF NOT EXISTS card_front_text_is_valid(text) AS
  length(text) > 0
  AND length(text) <= 200;

CREATE MACRO IF NOT EXISTS card_back_text_is_valid(text) AS
  length(text) > 0
  AND length(text) <= 500;

CREATE MACRO IF NOT EXISTS user_flag_is_valid(flag) AS
  flag BETWEEN 0 AND 7;

CREATE MACRO IF NOT EXISTS review_state_is_valid(state) AS
  state BETWEEN 0 AND 3;

CREATE MACRO IF NOT EXISTS target_language_code_is_valid(code) AS
  code BETWEEN 0 AND 66;

CREATE MACRO IF NOT EXISTS desired_retention_is_valid(value) AS
  value > 0.0
  AND value < 1.0;

CREATE MACRO IF NOT EXISTS maximum_interval_days_is_valid(value) AS
  value > 0;

CREATE MACRO IF NOT EXISTS fsrs_difficulty_is_valid(value) AS
  value >= 1.0
  AND value <= 10.0;

CREATE MACRO IF NOT EXISTS fsrs_stability_is_valid(value) AS
  value > 0.0;

CREATE MACRO IF NOT EXISTS review_rating_is_valid(value) AS
  value BETWEEN 1 AND 4;

CREATE MACRO IF NOT EXISTS elapsed_seconds_is_valid(value) AS
  value >= 0.0;

CREATE MACRO IF NOT EXISTS scheduled_seconds_is_valid(value) AS
  value IS NULL
  OR value >= 0.0;

CREATE TABLE IF NOT EXISTS fsrs_configs (
  id UUID PRIMARY KEY DEFAULT uuidv7(),
  name VARCHAR NOT NULL UNIQUE,
  algorithm_version VARCHAR NOT NULL,
  desired_retention DOUBLE NOT NULL CHECK (
    desired_retention_is_valid(desired_retention)
  ),
  maximum_interval_days UINTEGER NOT NULL CHECK (
    maximum_interval_days_is_valid(maximum_interval_days)
  ),
  parameters_json JSON NOT NULL,
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP
);

CREATE TABLE IF NOT EXISTS decks (
  id UUID PRIMARY KEY DEFAULT uuidv7(),
  name VARCHAR NOT NULL UNIQUE CHECK (
    deck_name_is_valid(name)
  ),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  target_language_code UTINYINT NOT NULL CHECK (
    target_language_code_is_valid(target_language_code)
  ),
  fsrs_config_id UUID NOT NULL REFERENCES fsrs_configs(id)
);

CREATE TABLE IF NOT EXISTS cards (
  id UUID PRIMARY KEY DEFAULT uuidv7(),
  deck_id UUID NOT NULL REFERENCES decks(id),
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP,
  last_reviewed_at TIMESTAMP,
  due_at TIMESTAMP,
  review_state UTINYINT NOT NULL DEFAULT 0 CHECK (
    review_state_is_valid(review_state)
  ),
  difficulty DOUBLE NOT NULL CHECK (
    fsrs_difficulty_is_valid(difficulty)
  ),
  stability DOUBLE NOT NULL CHECK (
    fsrs_stability_is_valid(stability)
  ),
  front_text VARCHAR NOT NULL CHECK (
    card_front_text_is_valid(front_text)
  ),
  front_normalized_text VARCHAR GENERATED ALWAYS AS (
    lower(strip_accents(nfc_normalize(front_text)))
  ),
  back_text VARCHAR NOT NULL CHECK (
    card_back_text_is_valid(back_text)
  ),
  back_normalized_text VARCHAR GENERATED ALWAYS AS (
    lower(strip_accents(nfc_normalize(back_text)))
  ),
  audio_path VARCHAR,
  user_flag UTINYINT NOT NULL DEFAULT 0 CHECK (
    user_flag_is_valid(user_flag)
  ),
  UNIQUE (deck_id, front_text)
);

CREATE TABLE IF NOT EXISTS card_reviews (
  id UUID PRIMARY KEY DEFAULT uuidv7(),
  card_id UUID NOT NULL REFERENCES cards(id),
  reviewed_at TIMESTAMP NOT NULL,
  rating UTINYINT NOT NULL CHECK (
    review_rating_is_valid(rating)
  ),
  elapsed_seconds DOUBLE NOT NULL CHECK (
    elapsed_seconds_is_valid(elapsed_seconds)
  ),
  scheduled_seconds DOUBLE CHECK (
    scheduled_seconds_is_valid(scheduled_seconds)
  ),
  review_state_before UTINYINT CHECK (
    review_state_is_valid(review_state_before)
  ),
  review_state_after UTINYINT CHECK (
    review_state_is_valid(review_state_after)
  ),
  stability_before DOUBLE CHECK (
    fsrs_stability_is_valid(stability_before)
  ),
  stability_after DOUBLE CHECK (
    fsrs_stability_is_valid(stability_after)
  ),
  difficulty_before DOUBLE CHECK (
    fsrs_difficulty_is_valid(difficulty_before)
  ),
  difficulty_after DOUBLE CHECK (
    fsrs_difficulty_is_valid(difficulty_after)
  )
);

COMMIT;
