(layer/resize 1024px 512px)
(layer/set-dpi 96)

(tools/plotgen
    bars (
        limit-x (0 24)
        limit-y (0 10)
        data-x (csv "test/testdata/gdp_per_capita_2010.csv" gdp)
        data-y (csv "test/testdata/gdp_per_capita_2010.csv" rank)
        labels (csv "test/testdata/gdp_per_capita_2010.csv" gdp_label)
        direction horizontal
        width (1.2em)
        color #666))
