IMPORT TABLE example_data
   FROM 'csv:examples/data/point_example.csv?headers=true';

DRAW POINTCHART WITH
   AXIS BOTTOM
   AXIS LEFT
   AXIS TOP
   AXIS RIGHT;

SELECT series AS series, x AS x, y AS y, z AS z
   FROM example_data;
