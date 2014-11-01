IMPORT TABLE example_data
   FROM 'csv:examples/data/areadata.csv?headers=true';

DRAW AREACHART WITH
   AXIS TOP
   AXIS BOTTOM
   AXIS RIGHT
   AXIS LEFT
   YDOMAIN 0, 50;

SELECT
      'data' AS series,
      x AS x,
      y AS y,
      'circle' as pointstyle,
      'solid' as linestyle,
      3 as pointsize,
      2 as linewidth
   FROM example_data
   WHERE series = "series1";
