(layer/resize 1024px 80px)
(layer/set-dpi 96)

(figure/plot
    margin 2em
    axis (
        label-format (datetime "%H:%M:%S")
        label-placement (linear 7200)
        limit (0 79200)))
