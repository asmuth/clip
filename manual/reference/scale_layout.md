Scale Layout
============

The scale layout controls the placement of labels and ticks on the scale. It
does *not* control how data is mapped to screen coordinates, which is instead
controlled by a separate [Scale Type](../scale-type) option or how labels are
formatted, which is controlled by the [Formatters](../format) option.


List of `<scale-layout>` values
-------------------------------

### `linear`

The `linear` layout will place a 'tick' every `N` units. If no `N` parameter
is provided it will be chosen automatically.

    (linear)
    (linear <N>)

Example: [`charts-reference/scale_layout_linear`](/examples/charts-reference/scale_layout_linear):
[![scale_layout_linear.svg](/examples/charts-reference/scale_layout_linear.svg)](/examples/charts-reference/scale_layout_linear)

---
### `subdivide`

The `subdivide` layout will divide the axis into N evenly sized divisions.

    (subdivide)
    (subdivide <N>)

Example: [`charts-reference/scale_layout_subdivide`](/examples/charts-reference/scale_layout_subdivide):
[![scale_layout_subdivide.svg](/examples/charts-reference/scale_layout_subdivide.svg)](/examples/charts-reference/scale_layout_subdivide)
