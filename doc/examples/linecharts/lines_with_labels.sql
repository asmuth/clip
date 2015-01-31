IMPORT TABLE city_temperatures
   FROM 'csv:examples/data/city_temperatures.csv?headers=true';

DRAW LINECHART WITH
   AXIS LEFT
   AXIS BOTTOM
   LABELS
   YDOMAIN -10, 40
   LEGEND TOP LEFT INSIDE;

SELECT city AS series, month AS x, temperature AS y, temperature + "°C" as label, "circle" as pointstyle
   FROM city_temperatures
   WHERE city = "Berlin";
