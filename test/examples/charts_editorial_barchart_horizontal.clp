(set-width 2048px)
(set-height 800px)
(set-dpi 240)

(default limit-x (0 24))
(default scale-y (categorical (csv "test/testdata/gdp_per_capita_2010.csv" country)))

(layout/add-margins margin 1em)

(plot/add-axes
    position (left))

(plot/draw-bars
    data-x (csv "test/testdata/gdp_per_capita_2010.csv" gdp)
    data-y (csv "test/testdata/gdp_per_capita_2010.csv" country)
    labels (csv "test/testdata/gdp_per_capita_2010.csv" gdp_label)
    direction horizontal
    bar-width (1.4em)
    color #4c7bb0)
