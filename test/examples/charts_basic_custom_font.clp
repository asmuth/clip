(width 1600px)
(height 800px)
(dpi 240)

(default font "Comic Sans MS")
(default limit-x (1404278100 1404299700))
(default limit-y (0 120))

(plot/axes
    position (bottom left)
    label-format-y (fixed 2)
    label-format-x (datetime "%H:%M:%S")
    label-placement-x (linear 3600)
    label-placement-y (linear 20))

(draw/rectangle
    fill #f2f2f2)

(plot/grid
    color #fff)

(plot/lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2))

(figure/legend
    position (top right)
    border none
    margin 0.5em
    item-flow on
    item (label "A very serious measurement"))
