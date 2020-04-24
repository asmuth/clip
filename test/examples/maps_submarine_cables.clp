(width 2048px)
(height 1200px)
(dpi 220)

(default limit-x (-180 180))
(default limit-y (-80 100))

(draw/rectangle
    fill #f9f9f9)

(plot/grid
    tick-placement-x (linear 10)
    tick-placement-y (none)
    color (rgba 0 0 0 .1)
    stroke-style (dashed)
    stroke-width .5pt)

(plot/lines
    data (geojson /home/paul/downloads/mapex/submarine_cables.geojson)
    stroke-color #666
    stroke-width .5pt)

(plot/polygons
    data (geojson /home/paul/downloads/mapex/world.geojson)
    fill #e8e8e8
    stroke-color #bbb
    stroke-width .2pt)

(figure/legend
    item (label "Submarine cables"))

