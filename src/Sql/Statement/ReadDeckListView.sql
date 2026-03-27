SELECT
  id::VARCHAR,
  name,
  due,
  total
FROM
  deck_list_view
ORDER BY
  name;
