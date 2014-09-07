IMPORT TABLE city_temperatures
   FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW LINECHART WITH
   AXIS BOTTOM TITLE "fnord"
   AXIS LEFT TITLE "fnord"
   AXIS TOP TITLE "fnord"
   AXIS RIGHT TITLE "fnord"
   LEGEND TOP RIGHT INSIDE;

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle
   FROM city_temperatures;
