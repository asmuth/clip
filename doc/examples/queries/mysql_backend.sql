IMPORT TABLE test_table
   FROM 'mysql://localhost/test_database?user=root';

-- my comment 
asd
DRAW BARCHART WITH
    ORIENTATION VERTICAL
    AXIS BOTTOM  -- another comment
    GRID HORIZONTAL
    LEGEND TOP RIGHT INSIDE;

SELECT series, x, y FROM test_table WHERE series = "fnord"; -- final comment

