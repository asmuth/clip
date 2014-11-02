IMPORT TABLE city_temperatures
    FROM 'csv:examples/data/city_temperatures.csv?headers=true';

DRAW BARCHART WITH
    ORIENTATION VERTICAL
    YDOMAIN 0, 50
    AXIS RIGHT
    AXIS TOP;

SELECT city AS series, month AS x, temperature AS y
    FROM city_temperatures
    WHERE city = "London";

DRAW LINECHART WITH
    YDOMAIN 0, 70
    AXIS BOTTOM
    AXIS LEFT;

SELECT city AS series, month AS x, temperature * 2 AS y, "circle" as pointstyle, "color2" as color
    FROM city_temperatures
    WHERE city = "Tokyo";
