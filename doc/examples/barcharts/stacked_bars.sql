IMPORT TABLE gdp_per_country
   FROM CSV 'test/fixtures/gbp_per_country_simple.csv' HEADER;

DRAW BARCHART WITH
  ORIENTATION HORIZONTAL
  STACKED
  AXIS LEFT
  AXIS BOTTOM;

SELECT
  'gross domestic product per country' AS series,
  country AS x,
  gbp AS y
FROM
  gdp_per_country
LIMIT 10;

SELECT
  'gross domestic product per country 2' AS series,
  country AS x,
  gbp AS y
FROM
  gdp_per_country
LIMIT 10;
