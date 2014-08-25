IMPORT TABLE city_temperatures
   FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW LINE CHART;
DRAW BOTTOM AXIS;

SELECT
  city AS series,
  month AS x,
  temperature AS y
FROM
  city_temperatures;
