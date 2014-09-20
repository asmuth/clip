IMPORT TABLE test_table
   FROM 'mysql://localhost/test_database?user=root';

DRAW BARCHART WITH
    ORIENTATION VERTICAL
    AXIS BOTTOM
    GRID HORIZONTAL
    LEGEND TOP RIGHT INSIDE;

SELECT series, "A" as x, y FROM test_table;

