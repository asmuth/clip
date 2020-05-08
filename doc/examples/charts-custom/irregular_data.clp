(width 2048px)
(height 800px)
(dpi 240)

(limit-x (-12 22))
(limit-y (-35 55))

(axes
    position (bottom left))

(lines
    data-x (csv "test/testdata/irregular_data1.csv" x)
    data-y (csv "test/testdata/irregular_data1.csv" y)
    color 4
    marker-size 3pt)

(lines
    data-x (csv "test/testdata/irregular_data2.csv" x)
    data-y (csv "test/testdata/irregular_data2.csv" y)
    color 6
    marker-size 3pt)

(legend
    position (bottom left)
    background #fff
    item (label "Series A" color 4)
    item (label "Series B" color 6))
