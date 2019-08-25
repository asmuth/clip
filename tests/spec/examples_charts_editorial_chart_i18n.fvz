(set width 2048px)
(set height 800px)
(set dpi 240)

(set margin-top 2em)

(plot
    scale-x-padding 0.6
    limit-y (-15 30)
    axes (bottom left)
    axis-x-label-placement (linear-alignat 1 1)
    axis-x-label-format (custom "Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec")
    axis-y-label-placement (linear-interval 5 -10 30)
    axis-y-title "Temperature (°C)"
    lines (
        data-x (csv "tests/testdata/city_temperatures.csv" month)
        data-y (csv "tests/testdata/city_temperatures.csv" temp_beijing)
        color #aa4643
        marker-size 4pt)
    lines (
        data-x (csv "tests/testdata/city_temperatures.csv" month)
        data-y (csv "tests/testdata/city_temperatures.csv" temp_moscow)
        color #89a54e
        marker-size 4pt)
    lines (
        data-x (csv "tests/testdata/city_temperatures.csv" month)
        data-y (csv "tests/testdata/city_temperatures.csv" temp_telaviv)
        color #4572a7
        marker-size 4pt)
    legend-overlay (
        position (bottom center)
        border none
        margin 0.5em
        item-flow on
        item (label "Москва" color #89a54e)
        item (label "תל אביב" color #4572a7)
        item (label "北京市" color #aa4643)))
