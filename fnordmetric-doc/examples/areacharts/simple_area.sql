IMPORT TABLE example_data
   FROM 'csv:examples/data/measurement.csv?headers=true';

DRAW AREACHART WITH
  AXIS BOTTOM
  AXIS LEFT
  GRID HORIZONTAL;

SELECT 'data' AS series, FROM_TIMESTAMP(time) AS x, value2 * 1000 AS y, 'solid' as linestyle
   FROM example_data
   WHERE series = "measurement2";
