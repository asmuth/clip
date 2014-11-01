IMPORT TABLE city_temperatures
    FROM 'csv:examples/data/city_temperatures.csv?headers=true';

DRAW LINECHART WITH
    AXIS BOTTOM
    AXIS LEFT TITLE "New York"
    LEGEND TOP LEFT INSIDE;

SELECT city AS series, month AS x, temperature AS y, "circle" AS pointstyle
    FROM city_temperatures
    WHERE city = "New York";

DRAW LINECHART WITH
    AXIS RIGHT TITLE "Berlin";

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle, "color2" as color
    FROM city_temperatures
    WHERE city = "Berlin";
