IMPORT TABLE example_data
   FROM 'csv:examples/data/areadata2.csv?headers=true';

DRAW AREACHART WITH
   AXIS LEFT
   AXIS BOTTOM
   YDOMAIN -50, 50;

SELECT
      series AS series,
      x AS x,
      y AS y,
      z AS z
   FROM example_data
   WHERE series = "series2" or series = "series1";

DRAW LINECHART
   AXIS TOP
   AXIS RIGHT
   YDOMAIN -50, 50;

SELECT
      series AS series,
      x AS x,
      y AS y,
      "black" as color,
      "circle" as pointstyle
   FROM example_data
   WHERE series = "series3";
