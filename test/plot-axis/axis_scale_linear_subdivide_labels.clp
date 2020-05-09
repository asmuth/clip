(layer/resize 1024px 512px)
(layer/set-dpi 96)

(tools/plotgen
    margin 8em
    axis (
        align x
        label-placement (subdivide 5)
        label-format (custom "-2σ" "-1σ" "0" "1σ" "2σ")
        limit (-10 10)))
