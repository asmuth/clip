IMPORT TABLE city_temperatures
   FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW LINECHART WITH
   AXIS LEFT
   AXIS BOTTOM
   LEGEND TOP LEFT INSIDE;

SELECT
   city AS series,
   month AS x,
   temperature AS y,
   "Avg. Temperatures in " + month + " in " + city + ": " + temperature as label,
   "circle" as pointstyle
   FROM city_temperatures;
