IMPORT TABLE example_data
   FROM 'csv:examples/data/areadata.csv?headers=true';

DRAW BARCHART
    ORIENTATION HORIZONTAL
    AXIS BOTTOM
    AXIS LEFT;

SELECT series as series, w as x, y as y, z as z from example_data WHERE series = "series1";
SELECT series as series, w as x, y + 20 as y, z + 20 as z from example_data WHERE series = "series2";
