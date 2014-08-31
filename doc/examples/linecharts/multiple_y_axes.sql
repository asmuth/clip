IMPORT TABLE city_temperatures
    FROM CSV 'doc/examples/data/city_temperatures.csv' HEADER;

DRAW LINECHART WITH
    AXIS BOTTOM
    AXIS LEFT;

SELECT city AS series, month AS x, temperature AS y, "circle" AS pointstyle
    FROM city_temperatures
    WHERE city = "New York";

DRAW LINECHART WITH
    AXIS RIGHT;

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle, "color2" as color
    FROM city_temperatures
    WHERE city = "Berlin";
