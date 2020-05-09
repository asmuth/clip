(layer/resize 1024px 60px)
(layer/set-dpi 96)

(tools/plotgen
    margin 1em
    axis (
        scale (logarithmic 2)
        limit (0 1024)))
