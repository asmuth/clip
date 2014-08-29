IMPORT TABLE gdp_per_country
   FROM CSV 'test/fixtures/gbp_per_country_simple.csv' HEADER;

DRAW BAR CHART;
DRAW LEFT AXIS;

SELECT
  'gross domestic product per country' AS series,
  country AS x,
  0 as y,
  gbp AS z
FROM
  gdp_per_country
LIMIT 30;
