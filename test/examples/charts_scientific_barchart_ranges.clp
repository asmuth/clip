(set-width 2048px)
(set-height 768px)
(set-dpi 240)

(default font "Latin Modern Roman")
(default scale-x (categorical (A B C D E F)))
(default limit-y (-10 70))

(layout/add-margins margin 1em)

(figure/draw-legend
    position (top center)
    item-flow on
    margin 0
    padding-bottom 1em
    item (label "Series A" marker-shape (custom "▢"))
    item (label "Series B" marker-shape (square-hatch-diag-right)))

(layout/add-margins margin-top 3em)

(plot/add-axes
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
    bar-width (1em)
    bar-offset (-.8em)
    bar-stroke-width .8pt
    bar-fill none)

(plot/draw-bars
    data-x (csv "test/testdata/bardata.csv" var3)
    data-y (csv "test/testdata/bardata.csv" var4)
    data-y-low (csv "test/testdata/bardata.csv" var5)
    bar-width (1em)
    bar-offset (.8em)
    bar-stroke-width .8pt
    bar-fill hatch)
