(height 480px)
(width 1200px)

(default limit-y (-50 50))

(plot/axes
    position (bottom top))

(plot/areas
    data-x (csv "test/testdata/areadata2.csv" x)
    data-y (csv "test/testdata/areadata2.csv" z)
    data-y-low (csv "test/testdata/areadata2.csv" y)
    color #ccc)

(plot/areas
    data-x (csv "test/testdata/areadata2.csv" x)
    data-y (csv "test/testdata/areadata2.csv" z2)
    data-y-low (csv "test/testdata/areadata2.csv" y2)
    color #888)

(plot/lines
    data-x (10 20 30 40 50)
    data-y (1.23 10.32 -6.23 4.43 3.45)
    marker-size 4pt)

(figure/legend
    position (bottom right)
    item-flow on
    item (label "Series A" color #ccc)
    item (label "Series B" color #888)
    item (label "Combined" color #000))
