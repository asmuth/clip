IMPORT TABLE city_temperatures
   FROM 'csv:doc/tests/testdata/city_temperatures.csv?headers=true';

DRAW LINECHART AXIS BOTTOM;

SELECT
  'Berlin' AS series,
  temperature AS x,
  temperature AS y
FROM
  city_temperatures
WHERE city = "Berlin";

SELECT
  'Tokyo' AS series,
  temperature AS x,
  temperature AS y
FROM
  city_temperatures
WHERE city = "Tokyo";
