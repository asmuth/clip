(set-width 2048px)
(set-height 800px)
(set-dpi 240)

(default limit-x (-12 22))
(default limit-y (-35 55))

(layout/add-margins margin 1em)

(plot/add-axes
    position (bottom left))

(plot/draw-lines
    data-x (csv "test/testdata/irregular_data1.csv" x)
    data-y (csv "test/testdata/irregular_data1.csv" y)
    color 4
    marker-size 3pt)

(plot/draw-lines
    data-x (csv "test/testdata/irregular_data2.csv" x)
    data-y (csv "test/testdata/irregular_data2.csv" y)
    color 6
    marker-size 3pt)

(figure/draw-legend
    position (bottom left)
    background #fff
    item (label "Series A" color 4)
    item (label "Series B" color 6))
