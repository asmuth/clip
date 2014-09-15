IMPORT TABLE gdp_per_country
   FROM 'csv:test/fixtures/gbp_per_country_simple.csv?headers=true';

DRAW BARCHART
  AXIS BOTTOM
  AXIS LEFT;

SELECT
  'gross domestic product per country' AS series,
  country AS x,
  gbp AS y
FROM
  gdp_per_country
LIMIT 30;
