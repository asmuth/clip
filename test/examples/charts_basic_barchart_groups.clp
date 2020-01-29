(width 2048px)
(height 512px)
(dpi 240)

(default scale-x (categorical (A B C D E F)))
(default limit-y (0 80))

(plot/axes
    label-placement-y (linear-interval 10 10 70))

(plot/grid
    color (rgba 0 0 0 .05)
    stroke-style (dashed)
    tick-placement-x (none))

(plot/bars
    data-x (csv "test/testdata/bardata.csv" var3)
    data-y (csv "test/testdata/bardata.csv" var1)
    width (1em)
    offset (-.8em)
    color #444)

(plot/bars
    data-x (csv "test/testdata/bardata.csv" var3)
    data-y (csv "test/testdata/bardata.csv" var2)
    width (1em)
    offset (.8em)
    color #999)
