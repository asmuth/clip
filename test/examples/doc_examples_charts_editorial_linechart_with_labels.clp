(set-width 2048px)
(set-height 800px)
(set-dpi 240)

(chart/linechart
    data-x (csv "test/testdata/city_temperatures_london.csv" month)
    data-y (csv "test/testdata/city_temperatures_london.csv" temperature)
    labels (csv "test/testdata/city_temperatures_london.csv" temperature_str)
    scale-x-padding 0.6
    axis-x-label-placement (linear-alignat 1 1)
    axis-x-label-format (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    limit-y (0 25)
    axis-y-label-placement (linear-alignat 5 -10)
    color 1
    marker-size 5pt
    axes (bottom)
    legend-overlay (
      item (label "London - Average Temperature" color 1)
      item-flow on
      border none))
