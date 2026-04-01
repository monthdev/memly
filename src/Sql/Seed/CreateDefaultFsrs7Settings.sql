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
  uuidv7 (),
  'Default',
  true,
  id,
  0.95,
  CURRENT_TIMESTAMP,
  NULL
FROM
  fsrs_schedulers
WHERE
  algorithm_version = 7
  AND is_version_default = true
  AND NOT EXISTS (
    SELECT
      1
    FROM
      fsrs_settings
    WHERE
      is_default = true
  )
LIMIT
  1;
