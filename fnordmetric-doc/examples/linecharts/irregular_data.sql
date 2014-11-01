IMPORT TABLE example_data
   FROM 'csv:examples/data/irregular_data.csv?headers=true';

DRAW LINECHART WITH
   AXIS BOTTOM
   AXIS LEFT
   AXIS TOP
   AXIS RIGHT;

SELECT series AS series, x AS x, y AS y, 'circle' as pointstyle
   FROM example_data;
