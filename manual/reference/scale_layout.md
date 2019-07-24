Scale Layout
============

The scale layout controls the placement of labels and ticks on the scale. It
does *not* control how data is mapped to screen coordinates, which is instead
controlled by a separate [Scale Type](../scale-type) option or how labels are
formatted, which is controlled by the [Formatters](../format) option.


List of `<scale-layout>` values
-------------------------------

### `linear`

The `linear` layout will place a 'tick' every `N` units. The first tick will be
placed at the `min` value of the scale.

    (linear <N>)

Example: [`charts-reference/scale_layout_linear`](/examples/charts-reference/scale_layout_linear):
[![scale_layout_linear.svg](/examples/charts-reference/scale_layout_linear.svg)](/examples/charts-reference/scale_layout_linear)

---
### `linear-align`

The `linear` layout will place a 'tick' every `N` units. The ticks will be placed
so that the modulo of  N and the value at each tick will be zero.

    (linear-align <N>)

Example: [`charts-reference/scale_layout_linear_align`](/examples/charts-reference/scale_layout_linear_align):
[![scale_layout_linear_align.svg](/examples/charts-reference/scale_layout_linear_align.svg)](/examples/charts-reference/scale_layout_linear_align)

---
### `linear-alignat`

The `linear` layout will place a 'tick' every `N` units. The ticks will be placed
so that one tick the `A` value has an exactly matching tick/label.

    (linear-alignat <N> <A>)

Example: [`charts-reference/scale_layout_linear_alignat`](/examples/charts-reference/scale_layout_linear_alignat):
[![scale_layout_linear_alignat.svg](/examples/charts-reference/scale_layout_linear_alignat.svg)](/examples/charts-reference/scale_layout_linear_alignat)

---
### `linear-interval`

The `linear` layout will place a 'tick' every `N` units. The ticks will be placed
starting at `B` through to `E`.

    (linear-interval <N> <B> <E>)

Example: [`charts-reference/scale_layout_linear_interval`](/examples/charts-reference/scale_layout_linear_interval):
[![scale_layout_linear_interval.svg](/examples/charts-reference/scale_layout_linear_interval.svg)](/examples/charts-reference/scale_layout_linear_interval)

---
### `subdivide`

The `subdivide` layout will divide the axis into N evenly sized divisions.

    (subdivide)
    (subdivide <N>)

Example: [`charts-reference/scale_layout_subdivide`](/examples/charts-reference/scale_layout_subdivide):
[![scale_layout_subdivide.svg](/examples/charts-reference/scale_layout_subdivide.svg)](/examples/charts-reference/scale_layout_subdivide)

---
### `categorical`

The `categorical` layout is used to place labels or ticks for categorical 
scales.

    (categorical)

Example: [`charts-reference/scale_layout_categorical`](/examples/charts-reference/scale_layout_categorical):
[![scale_layout_categorical.svg](/examples/charts-reference/scale_layout_categorical.svg)](/examples/charts-reference/scale_layout_categorical)

---
### `categorical-bounds`

The `categorical-bounds` layout is used to place ticks at the boundaries between
categories.

    (categorical-bounds)

Example: [`charts-reference/scale_layout_categorical_bounds`](/examples/charts-reference/scale_layout_categorical_bounds):
[![scale_layout_categorical_bounds.svg](/examples/charts-reference/scale_layout_categorical_bounds.svg)](/examples/charts-reference/scale_layout_categorical_bounds)
