INSERT INTO
  fsrs_configurations (
    id,
    name,
    algorithm_version,
    desired_retention,
    maximum_interval_days,
    parameters_json,
    created_at,
    updated_at
  )
SELECT
  uuidv7 (),
  'Default',
  'fsrs-7-recency',
  0.90,
  36500,
  json(
    '[0.041,2.4175,4.1283,11.9709,5.6385,0.4468,3.262,2.3054,0.1688,1.3325,0.3524,0.0049,0.7503,0.0896,0.6625,1.3,0.882,0.3072,3.5875,0.303,0.0107,0.2279,2.6413,0.5594,1.3,2.5,1.0,0.0723,0.1634,0.5,0.9555,0.2245,0.6232,0.1362,0.3862]'
  ),
  CURRENT_TIMESTAMP,
  NULL
WHERE
  NOT EXISTS (
    SELECT
      1
    FROM
      fsrs_configurations
    WHERE
      name = 'Default'
  );
