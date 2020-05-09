(layer/resize 1024px 512px)
(layer/set-dpi 96)

(tools/plotgen
    points (
        scale-x (categorical ("a" "b" "c" "d" "e"))
        limit-y (0 6)
        data-x ("a" "b" "c" "d" "e")
        data-y (1 2 3 4 5)))
