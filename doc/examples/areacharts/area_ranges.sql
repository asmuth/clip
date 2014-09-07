IMPORT TABLE example_data
   FROM CSV 'doc/examples/data/areadata2.csv' HEADER;

DRAW AREACHART WITH
   AXIS TOP
   AXIS BOTTOM
   AXIS RIGHT
   AXIS LEFT;

SELECT
      series AS series,
      x AS x,
      y AS y,
      z AS z
   FROM example_data
   WHERE series = "series2" or series = "series1";
