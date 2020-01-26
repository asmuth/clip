(set-width 2048px)
(set-height 768px)
(set-dpi 240)
(default font "Latin Modern Roman")

(layout/add-margins margin 1em)

(default limit-x (0 7200))

(plot/add-axes
    axis-x-label-format (datetime "%H:%M:%S")
    axis-x-label-placement (linear-interval 900 900 7000)
    axis-left-limit (0 100)
    axis-left-label-placement (linear-interval 10 10 50)
    axis-right-limit (0 1)
    axis-right-label-placement (linear-interval 0.1 0.5 1.0))

(plot/draw-grid
    stroke-color (rgba 0 0 0 .2)
    stroke-style dashed
    tick-placement-x (none))

(plot/draw-lines
    data-x (csv "test/testdata/measurement2.csv" time)
    data-y (csv "test/testdata/measurement2.csv" value3)
    limit-y (-4 10)
    stroke-width 0.8pt)

(plot/draw-lines
    data-x (csv "test/testdata/measurement2.csv" time)
    data-y (csv "test/testdata/measurement2.csv" value3)
    limit-y (-16 6)
    stroke-width 0.8pt)
