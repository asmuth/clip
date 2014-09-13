IMPORT TABLE example_data
   FROM CSV 'doc/examples/data/measurement.csv' HEADER;

DRAW LINECHART WITH
  AXIS BOTTOM
  AXIS LEFT;

SELECT 'data' AS series, FROM_TIMESTAMP(time) AS x, value2 * 1000 AS y
   FROM example_data
   WHERE series = "measurement2";
