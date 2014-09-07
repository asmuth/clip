IMPORT TABLE example_data
   FROM CSV 'doc/examples/data/areadata.csv' HEADER;

DRAW BARCHART
    ORIENTATION HORIZONTAL
    AXIS BOTTOM
    AXIS LEFT;

SELECT series as series, w as x, y as y, z as z from example_data WHERE series = "series1";
SELECT series as series, w as x, y + 20 as y, z + 20 as z from example_data WHERE series = "series2";
