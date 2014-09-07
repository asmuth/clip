IMPORT TABLE gdp_per_capita
   FROM CSV 'doc/examples/data/gdp_per_capita.csv' HEADER;

SELECT 'gdp per country' AS series, isocode, gdp AS y, year as year
    FROM gdp_per_capita
    WHERE year = "2010"
    LIMIT 100;
