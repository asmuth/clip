(width 2048px)
(height 1200px)
(dpi 240)

(default limit-x (1 5))
(default limit-y (0 10000))
(default scale-y (log))

(plot/axes
    label-format-y (base 10))

(plot/grid color (rgba 0 0 0 .05))

(plot/lines
    data-x (csv "test/testdata/log_example.csv" x)
    data-y (csv "test/testdata/log_example.csv" y))
