IMPORT TABLE city_temperatures
   FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW LINECHART WITH
   TITLE "mytitle"
   SUBTITLE "fnordtitle"
   AXIS BOTTOM
   AXIS LEFT
   LEGEND BOTTOM LEFT INSIDE TITLE "Average Temperature"
   YDOMAIN -30.0, 30.0;

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle
   FROM city_temperatures;
