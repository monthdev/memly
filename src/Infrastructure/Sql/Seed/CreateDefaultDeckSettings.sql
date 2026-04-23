INSERT INTO
  deck_settings (
    id,
    name,
    is_default,
    fsrs_settings_id,
    scheduler_type,
    created_at,
    updated_at
  )
SELECT
  UUIDV7 (),
  'Default',
  TRUE,
  id,
  'fsrs',
  CURRENT_TIMESTAMP,
  NULL
FROM
  fsrs_settings
WHERE
  is_default = TRUE
  AND NOT EXISTS (
    SELECT
      1
    FROM
      deck_settings
    WHERE
      is_default = TRUE
  )
LIMIT
  1;
