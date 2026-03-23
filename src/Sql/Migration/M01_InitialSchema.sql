BEGIN TRANSACTION;

CREATE MACRO IF NOT EXISTS deck_name_is_valid (Name) AS length(Name) > 0
AND length(Name) <= 40;

CREATE MACRO IF NOT EXISTS target_language_code_is_valid (Code) AS (
  length(Code) = 2
  AND Code = lower(Code)
)
OR Code IN ('zh-cn', 'zh-tw', 'pt-pt');

CREATE MACRO IF NOT EXISTS card_front_text_is_valid (Text) AS length(Text) > 0
AND length(Text) <= 200;

CREATE MACRO IF NOT EXISTS card_back_text_is_valid (Text) AS length(Text) > 0
AND length(Text) <= 500;

CREATE MACRO IF NOT EXISTS user_flag_is_valid (Flag) AS Flag BETWEEN 0 AND 7;

CREATE TABLE IF NOT EXISTS decks (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  name VARCHAR NOT NULL UNIQUE CHECK (deck_name_is_valid (name)),
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  target_language_code VARCHAR NOT NULL CHECK (
    target_language_code_is_valid (target_language_code)
  )
);

CREATE TABLE IF NOT EXISTS cards (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  deck_id UUID NOT NULL REFERENCES decks (id),
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP,
  reviewed_at TIMESTAMP,
  difficulty DOUBLE NOT NULL,
  stability DOUBLE NOT NULL,
  front_text VARCHAR NOT NULL CHECK (card_front_text_is_valid (front_text)),
  front_normalized_text VARCHAR GENERATED ALWAYS AS (lower(strip_accents(nfc_normalize(front_text)))),
  back_text VARCHAR NOT NULL CHECK (card_back_text_is_valid (back_text)),
  back_normalized_text VARCHAR GENERATED ALWAYS AS (lower(strip_accents(nfc_normalize(back_text)))),
  audio_path VARCHAR,
  user_flag TINYINT NOT NULL DEFAULT 0 CHECK (user_flag_is_valid (user_flag)),
  UNIQUE (deck_id, front_text)
);

COMMIT;
