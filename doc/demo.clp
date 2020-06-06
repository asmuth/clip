(size 2048px 768px)
(dpi 240)
(font "Latin Modern Roman")
(limit-x (0 7200))
(limit-y (0 100))

(axes
    label-format-x (datetime "%H:%M:%S")
    label-placement-x (linear-interval 900 900 7000))

(grid
    stroke-color (rgba 0 0 0 0.2)
    stroke-style dashed
    tick-placement-x (none))

(lines
    data-x (csv "test/testdata/timeseries.csv" time)
    data-y (csv "test/testdata/timeseries.csv" value)
    stroke-width 0.8pt)
