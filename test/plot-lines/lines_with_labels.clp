(layer/resize 1200px 480px)
(layer/set-dpi 96)

(tools/plotgen
    lines (
        data-x (csv "test/testdata/city_temperatures_london.csv" month)
        data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
        labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
        color #06c
        marker-size 3pt
        scale-x-padding 0.6
        limit-y (-10 32)))
