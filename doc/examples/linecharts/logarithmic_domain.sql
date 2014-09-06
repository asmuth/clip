IMPORT TABLE city_temperatures
   FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW LINECHART WITH
   YDOMAIN LOGARITHMIC
   AXIS BOTTOM
   AXIS LEFT;

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle
   FROM city_temperatures;
