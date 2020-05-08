(height 480px)
(width 1200px)

(lines    
    data-x (csv "test/testdata/measurement.csv" time)
    data-y (csv "test/testdata/measurement.csv" value2)
    color #06c
    scale-y (inverted)
    limit-y (0 140))
