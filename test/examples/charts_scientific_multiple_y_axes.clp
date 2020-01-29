(width 2048px)
(height 768px)
(dpi 240)
(default font "Latin Modern Roman")

(default limit-x (0 7200))

(plot/axes
    label-format-x (datetime "%H:%M:%S")
    label-placement-x (linear-interval 900 900 7000)
    limit-left (0 100)
    label-placement-left (linear-interval 10 10 50)
    limit-right (0 1)
    label-placement-right (linear-interval 0.1 0.5 1.0))

(plot/grid
    stroke-color (rgba 0 0 0 .2)
    stroke-style dashed
    tick-placement-x (none))

(plot/lines
    data-x (csv "test/testdata/measurement2.csv" time)
    data-y (csv "test/testdata/measurement2.csv" value3)
    limit-y (-4 10)
    stroke-width 0.8pt)

(plot/lines
    data-x (csv "test/testdata/measurement2.csv" time)
    data-y (csv "test/testdata/measurement2.csv" value3)
    limit-y (-16 6)
    stroke-width 0.8pt)
