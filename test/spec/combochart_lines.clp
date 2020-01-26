(set height 480px)
(set width 1200px)

(plot
    scale-x-padding 0.6
    limit-y (-20 35)
    axes (bottom left)
    label-placement-x (linear-align 1)
    label-format-x (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    label-placement-y (linear-align 5)
    lines (
        data-x (csv "test/testdata/city_temperatures.csv" month)
        data-y (csv "test/testdata/city_temperatures.csv" temp_nyc)
        color #4572a7
        marker-size 3pt)
    lines (
        data-x (csv "test/testdata/city_temperatures.csv" month)
        data-y (csv "test/testdata/city_temperatures.csv" temp_beijing)
        color #aa4643
        marker-size 3pt)
    lines (
        data-x (csv "test/testdata/city_temperatures.csv" month)
        data-y (csv "test/testdata/city_temperatures.csv" temp_moscow)
        color #89a54e
        marker-size 3pt)
    lines (
        data-x (csv "test/testdata/city_temperatures.csv" month)
        data-y (csv "test/testdata/city_temperatures.csv" temp_berlin)
        color #80699b
        marker-size 3pt)
    legend-overlay (
        position (bottom left)
        item (label "New York" color #4572a7)
        item (label "北京市   " color #aa4643)
        item (label "Москва" color #89a54e)
        item (label "Berlin" color #80699b)))
