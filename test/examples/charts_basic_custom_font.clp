(set-width 2048px)
(set-height 512px)
(set-dpi 240)

(layout/add-margins margin 1em)

(default font "Comic Sans MS")
(default limit-x (1404278100 1404299700))
(default limit-y (0 120))

(plot/add-axes
    position (bottom left)
    axis-y-label-format (fixed 2)
    axis-x-label-format (datetime "%H:%M:%S")
    axis-y-label-placement (linear 20))

(plot/draw-lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2))

(figure/draw-legend
    position (top right)
    border none
    margin 0.5em
    item-flow on
    item (label "A very serious measurement"))
