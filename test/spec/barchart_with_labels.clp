(plot
    limit-x (0 13)
    limit-y (0 25)
    axes (top right bottom left)
    label-placement-x (linear-align 1)
    label-placement-x (linear-align 1)
    label-format-x (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    bars (
      data-x (csv "test/testdata/city_temperatures_london.csv" month)
      data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
      labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
      bar-width (2em)
      color #666))
