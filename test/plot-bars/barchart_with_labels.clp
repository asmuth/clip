(bars
    limit-x (0 13)
    limit-y (0 25)
    data-x (csv "test/testdata/city_temperatures_london.csv" month)
    data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
    labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
    width (2em)
    color #666)
