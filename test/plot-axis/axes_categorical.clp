(layer/resize 1024px 512px)
(layer/set-dpi 96)

(tools/plotgen
    axes (
        scale-x (categorical (A B C D E F))
        scale-y (categorical (W X Y Z))))
