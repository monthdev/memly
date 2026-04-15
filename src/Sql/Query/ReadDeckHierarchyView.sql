SELECT
  id::VARCHAR,
  COALESCE(parent_id::VARCHAR, ''),
  name,
  due_now,
  by_today,
  total
FROM
  deck_hierarchy_view;
