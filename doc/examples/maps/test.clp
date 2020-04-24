(layer-resize 2048px 1536px)
(layer-set-dpi 320)

(draw-rectangle
    fill #eff7fe)

(draw-grid
    limit-x (-100 -77)
    limit-y (16 32)
    tick-placement-x (linear 5)
    tick-placement-y (linear 5)
    color (rgba 0 0 0 .1)
    stroke-style (dashed))

(draw-polygons
    data (geojson test/testdata/gulf_of_mexico.geojson)
    limit-x (-100 -77)
    limit-y (16 32)
    fill #ffefdb
    stroke-color #999187
    stroke-width .7pt)

(draw-text
    position (90 23)
    font-size 14pt
    color (rgba 0 0 0 .6)
    text "Gulf of Mexico")

(draw-axes
    padding 32px
    limit-x (-100 -77)
    limit-y (16 32)
    label-placement-x (linear 5)
    label-placement-y (linear 1)
    title-bottom "Longitude"
    title-left "Lattitude")

(layer-extend-top 40px)

(draw-text position (50% 20px) text "Title")
