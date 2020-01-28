(set-width 2048px)
(set-height 768px)
(set-dpi 240)

(default font "Latin Modern Roman")
(default scale-x (categorical (A B C D E F)))
(default limit-y (-10 70))

(figure/draw-legend
    position (top center)
    item-flow on
    padding-bottom 1em
    item (label "Series A" marker-shape (custom "▢"))
    item (label "Series B" marker-shape (square-hatch-diag-right)))

(plot/add-axes
    margin-top 3.6em
    label-placement-top (none)
    label-placement-y (linear-interval 10 0 60))

(plot/draw-grid
    stroke-color #000
    stroke-style dashed
    tick-placement-x (categorical-bounds)
    tick-placement-y (none))

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var3)
    data-y (csv "test/testdata/bardata.csv" var1)
    data-y-low (csv "test/testdata/bardata.csv" var2)
    width (1em)
    offset (-.8em)
    stroke-width .8pt
    fill none)

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var3)
    data-y (csv "test/testdata/bardata.csv" var4)
    data-y-low (csv "test/testdata/bardata.csv" var5)
    width (1em)
    offset (.8em)
    stroke-width .8pt
    fill hatch)
