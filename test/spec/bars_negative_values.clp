(plot
    limit-x (0 13)
    scale-y-padding 5
    axes (top right bottom left)
    label-placement-x (linear-align 1)
    label-placement-x (linear-align 1)
    label-format-x (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    bars (
      data-x (csv "test/testdata/city_temperatures.csv" month)
      data-y (csv "test/testdata/city_temperatures.csv" temp_moscow)
      bar-width (2em)
      color #666))
