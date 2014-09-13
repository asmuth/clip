IMPORT TABLE example_data
   FROM CSV 'doc/examples/data/point_example.csv' HEADER;

DRAW POINTCHART WITH
   AXIS BOTTOM
   AXIS LEFT
   AXIS TOP
   AXIS RIGHT
   GRID HORIZONTAL VERTICAL;

SELECT series AS series, x AS x, y AS y
   FROM example_data;
