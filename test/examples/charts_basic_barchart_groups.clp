(layer/resize 2048px 512px)
(layer/set-dpi 240)

(figure/plot
    scale-x (categorical (A B C D E F))
    limit-y (0 80)
    axes (
        label-placement-y (linear-interval 10 10 70))
    grid (
        color (rgba 0 0 0 0.05)
        stroke-style (dashed)
        tick-placement-x (none))
    bars (
        data-x (csv "test/testdata/bardata.csv" var3)
        data-y (csv "test/testdata/bardata.csv" var1)
        width (1em)
        offset (-.8em)
        color #444)
    bars (
        data-x (csv "test/testdata/bardata.csv" var3)
        data-y (csv "test/testdata/bardata.csv" var2)
        width (1em)
        offset (.8em)
        color #999))
