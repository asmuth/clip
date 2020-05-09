(layer/resize 2048px 1200px)
(layer/set-dpi 240)

(tools/plotgen
    limit-x (1 5)
    limit-y (0 10000)
    scale-y (log)
    lines (
        data-x (csv "test/testdata/log_example.csv" x)
        data-y (csv "test/testdata/log_example.csv" y)))
