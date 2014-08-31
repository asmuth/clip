IMPORT TABLE city_temperatures
   FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW BAR CHART WITH
  ORIENTATION VERTICAL;

DRAW LEFT AXIS;

SELECT
  city AS series,
  month AS x,
  temperature AS y
FROM
  city_temperatures;


