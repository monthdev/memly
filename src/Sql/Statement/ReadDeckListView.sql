SELECT
  id::VARCHAR,
  name,
  due_now,
  by_today,
  total
FROM
  deck_list_view
ORDER BY
  name;
