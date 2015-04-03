IMPORT TABLE city_temperatures
   FROM 'csv:examples/data/city_temperatures.csv?headers=true';

DRAW LINECHART WITH
   XDOMAIN INVERT
   YDOMAIN INVERT
   AXIS BOTTOM
   AXIS LEFT
   LEGEND BOTTOM RIGHT INSIDE;

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle
   FROM city_temperatures;
