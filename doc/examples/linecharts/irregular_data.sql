IMPORT TABLE example_data
   FROM CSV 'doc/examples/data/irregular_data.csv' HEADER;

DRAW LINECHART WITH
   AXIS BOTTOM
   AXIS LEFT
   AXIS TOP
   AXIS RIGHT;

SELECT series AS series, x AS x, y AS y, 'circle' as pointstyle
   FROM example_data;
