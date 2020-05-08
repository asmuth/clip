(width 2048px)
(height 800px)
(dpi 240)

(limit-y (0 70))
(limit-x (-0.05 2.05))

(axes
    position (bottom)
    label-placement-x (linear-interval 0.2 0 2.1))

(bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y4)
    width (1em)
    color #9fc3f5)

(bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y3)
    width (1em)
    color #88ade0)

(bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y2)
    width (1em)
    color #6393c9)

(bars
    data-x (csv "test/testdata/stacked.csv" x)
    data-y (csv "test/testdata/stacked.csv" y1)
    width (1em)
    color #4c7bb0)

(legend
    item (label "Variable D" color #9fc3f5)
    item (label "Variable C" color #88ade0)
    item (label "Variable B" color #6393c9)
    item (label "Variable A" color #4c7bb0))
