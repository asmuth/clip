(set-width 2048px)
(set-height 800px)
(set-dpi 240)

(default limit-y (0 70))
(default limit-x (-0.05 2.05))

(layout/add-margins
    margin 1em)

(plot/add-axes
    position (bottom)
    label-placement-x (linear-interval 0.2 0 2.1))

(plot/draw-bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y4)
    bar-width (1em)
    color #9fc3f5)

(plot/draw-bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y3)
    bar-width (1em)
    color #88ade0)

(plot/draw-bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y2)
    bar-width (1em)
    color #6393c9)

(plot/draw-bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y1)
    bar-width (1em)
    color #4c7bb0)

(figure/draw-legend
    item (label "Variable D" color #9fc3f5)
    item (label "Variable C" color #88ade0)
    item (label "Variable B" color #6393c9)
    item (label "Variable A" color #4c7bb0))
