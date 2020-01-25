(set-width 2048px)
(set-height 1200px)
(set-dpi 240)

(layout/add-margins margin 2em)

(default limit-x (1404278100 1404299700))
(default limit-y (0 1))

(plot/add-axes
    axis-left-label-placement (subdivide 4)
    axis-left-label-format (scientific)
    axis-left-label-attach bottom
    axis-left-label-rotate -90
    axis-bottom-label-format (datetime "%H:%M:%S")
    axis-bottom-label-attach left
    axis-bottom-label-rotate 45)

(plot/draw-lines
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value1))

