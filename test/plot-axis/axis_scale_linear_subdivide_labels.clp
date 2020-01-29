(set-height 60px)
(layout/add-margins margin 1em)

(plot/draw-axis
    align x
    label-placement (subdivide 5)
    label-format (custom "-2σ" "-1σ" "0" "1σ" "2σ")
    limit (-10 10))
