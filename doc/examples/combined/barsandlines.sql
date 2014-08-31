IMPORT TABLE city_temperatures
   FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW BARCHART WITH
  ORIENTATION VERTICAL
  AXIS RIGHT
  AXIS TOP;

SELECT
  city AS series,
  month AS x,
  temperature AS y
FROM
  city_temperatures
WHERE
  city = "London";

DRAW LINECHART WITH
  AXIS BOTTOM
  AXIS LEFT;

SELECT
  city AS series,
  month AS x,
  temperature AS y
FROM
  city_temperatures
WHERE
  city = "Tokyo";
