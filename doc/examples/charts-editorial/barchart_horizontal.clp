(width 2048px)
(height 800px)
(dpi 240)

(default limit-x (0 24))
(default scale-y (categorical (csv "test/testdata/gdp_per_capita_2010.csv" country)))

(plot/axes
    position (left))

(plot/bars
    data-x (csv "test/testdata/gdp_per_capita_2010.csv" gdp)
    data-y (csv "test/testdata/gdp_per_capita_2010.csv" country)
    labels (csv "test/testdata/gdp_per_capita_2010.csv" gdp_label)
    direction horizontal
    width (1.4em)
    color #4c7bb0)
