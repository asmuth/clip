(layer/resize 1024px 512px)
(layer/set-dpi 96)

(figure/plot
    bars (
        limit-x (0 13)
        scale-y-padding 5
        data-x (csv "test/testdata/city_temperatures.csv" month)
        data-y (csv "test/testdata/city_temperatures.csv" temp_moscow)
        width 2em
        color #666))
