(layer/resize 1024px 512px)
(layer/set-dpi 96)

(tools/plotgen
    points (
        scale-x (categorical ("blah" "fu"))
        data-x ("a" "b" "c" "d" "e")
        data-y (200px 300px 200px 300px 100px)))
