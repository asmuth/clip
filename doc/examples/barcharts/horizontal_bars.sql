IMPORT TABLE gdp_per_capita
   FROM CSV 'doc/examples/data/gdp_per_capita.csv' HEADER;

DRAW BARCHART WITH
    ORIENTATION HORIZONTAL
    AXIS LEFT
    AXIS BOTTOM;

SELECT 'gdp per country' AS series, isocode AS x, gdp AS y
    FROM gdp_per_capita
    WHERE year = "2010"
    LIMIT 20;
