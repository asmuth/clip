(layer/resize 1200px 600px)
(layer/set-dpi 96)

(tools/plotgen
    legend (
        item-flow on
        item (label "Fnord Test 1" color #06c)
        item (label "Fnord Test 2" color #c06)
        item (label "Fnord Test 3" color #c06)
        item (label "Fnord Test 4" color #6c0)))
