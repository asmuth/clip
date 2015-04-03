IMPORT TABLE gdp_per_capita
   FROM 'csv:examples/data/gdp_per_capita.csv?headers=true';

DRAW BARCHART WITH
    ORIENTATION VERTICAL
    AXIS BOTTOM
    GRID HORIZONTAL
    LEGEND TOP RIGHT INSIDE;

SELECT year AS series, isocode AS x, gdp AS y
    FROM gdp_per_capita
    WHERE year = "2010" OR year = '2009' or year = '2008'
    ORDER BY gdp DESC
    LIMIT 9;

