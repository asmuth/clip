IMPORT TABLE gdp_per_country
   FROM 'csv:test/fixtures/gbp_per_country_simple.csv?headers=true';

SELECT
  'gross domestic product per country' AS series,
  country AS x,
  0 as y,
  gbp AS z
FROM
  gdp_per_country
LIMIT 10;

SELECT
  'gross domestic product per country series2' AS series,
  country AS x,
  383700 as y,
  gbp AS z
FROM
  gdp_per_country
LIMIT 10;

SELECT
  'gross domestic product per country series3' AS series,
  country AS x,
  183700 as y,
  gbp AS z
FROM
  gdp_per_country
LIMIT 10;

SELECT
  'gross domestic product per country series4' AS series,
  country AS x,
  283700 as y,
  gbp AS z
FROM
  gdp_per_country
LIMIT 10;
