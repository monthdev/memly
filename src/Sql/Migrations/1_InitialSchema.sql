BEGIN TRANSACTION;



CREATE MACRO IF NOT EXISTS deck_name_length_ok (s) AS length(s) > 0
AND length(s) <= 25;

CREATE MACRO IF NOT EXISTS card_text_length_ok (s) AS length(s) > 0
AND length(s) <= 100;

CREATE TABLE IF NOT EXISTS migrations (
  version UINTEGER PRIMARY KEY,
  applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS settings (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  target_language_idx UINTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS decks (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  name VARCHAR NOT NULL UNIQUE CHECK (deck_name_length_ok (name))
);

CREATE TABLE IF NOT EXISTS cards (
  id UUID PRIMARY KEY DEFAULT uuidv7 (),
  deck_id UUID NOT NULL REFERENCES decks (id),
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP,
  reviewed_at TIMESTAMP,
  difficulty DOUBLE NOT NULL,
  stability DOUBLE NOT NULL,
  front_text VARCHAR NOT NULL CHECK (card_text_length_ok (front_text)),
  front_norm_text VARCHAR GENERATED ALWAYS AS (lower(strip_accents(nfc_normalize(front_text)))),
  back_text VARCHAR NOT NULL CHECK (card_text_length_ok (back_text)),
  back_norm_text VARCHAR GENERATED ALWAYS AS (lower(strip_accents(nfc_normalize(back_text)))),
  audio_path VARCHAR,
  UNIQUE (deck_id, front_text)
);

COMMIT;
