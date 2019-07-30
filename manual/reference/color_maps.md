Color Maps
==========

The `<color-map>` setting allows you to map input data to colors. These are the
currently implemented mapping methods:

  - `steps` -- Map values in [0..1] to a set of discrete colors
  - `gradient` -- Map values in [0..1] to a linear color gradient

#### Syntax:

    ;; a 'step' color map
    (steps (<step> <color>)...)

    ;; a 'gradient' color map
    (gradient (<step> <color>)...)


#### Examples:

    ;; gradient from red to blue
    color-map (gradient (0 #f00) (1.0 #00f))

    ;; step from red to green, blue, red, green to blue
    color-map (steps (0 #f00) (0.2 #0f0) (0.4 #00f) (0.6 #0f0) (0.8 #f00) (1.0 #00f))

For another example, see:
[`examples/charts-basic/scatterplot_colors`](/examples/charts-basic/scatterplot_colors)


