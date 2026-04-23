INSERT INTO
  fsrs_settings (
    id,
    name,
    is_default,
    fsrs_scheduler_id,
    desired_retention,
    created_at,
    updated_at
  )
SELECT
  UUIDV7 (),
  'Default',
  TRUE,
  id,
  0.95,
  CURRENT_TIMESTAMP,
  NULL
FROM
  fsrs_schedulers
WHERE
  algorithm_version = 7
  AND is_version_default = TRUE
  AND NOT EXISTS (
    SELECT
      1
    FROM
      fsrs_settings
    WHERE
      is_default = TRUE
  )
LIMIT
  1;
