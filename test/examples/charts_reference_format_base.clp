(layer/resize 1024px 80px)
(layer/set-dpi 96)

(figure/plot
    margin 2em
    axis (
        scale (logarithmic 2)
        label-format (base 2)
        limit (0 4096)))
