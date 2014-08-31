IMPORT TABLE gdp_per_country
   FROM CSV 'test/fixtures/gbp_per_country_simple.csv' HEADER;

DRAW BAR CHART WITH
  ORIENTATION VERTICAL;

DRAW LEFT AXIS;

SELECT
  'gross domestic product per country' AS series,
  country AS x,
  gbp AS y
FROM
  gdp_per_country
LIMIT 10;
