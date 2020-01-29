(set-width 2048px)
(set-height 1200px)
(set-dpi 240)

(default limit-x (1 5))
(default limit-y (0 10000))
(default scale-y (log))

(plot/draw-lines
    data-x (csv "test/testdata/log_example.csv" x)
    data-y (csv "test/testdata/log_example.csv" y))
