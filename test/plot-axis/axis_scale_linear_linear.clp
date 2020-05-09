(layer/resize 1024px 512px)
(layer/set-dpi 96)

(tools/plotgen
    margin 8em
    axis (
        align x
        label-placement (linear 1)
        tick-offset 0
        limit (0 16)))
