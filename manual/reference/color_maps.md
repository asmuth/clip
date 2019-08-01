Color Maps
==========

The `<color-map>` setting allows you to map input data to colors. These are the
currently implemented mapping methods:

  - `steps` -- Map values in [0..1] to a set of discrete colors
  - `gradient` -- Map values in [0..1] to a linear color gradient

### Syntax:

    ;; a 'step' color map
    (steps (<step> <color>)...)

    ;; a 'gradient' color map
    (gradient (<step> <color>)...)


### Example

Below you can see simple scatter plot that uses a red to blue color map to display a third
data dimension. The full source for this example is at:
[`examples/charts-scientific/scatterplot_colors`](/examples/charts-scientific/scatterplot_colors)

[![scatterplot_colors.svg](/examples/charts-scientific/scatterplot_colors.svg)](/examples/charts-scientific/scatterplot_colors)

The color map specification used in the example is:

    ;; gradient from gray to black
    color-map (gradient (0 #aaa) (1.0 #000))



