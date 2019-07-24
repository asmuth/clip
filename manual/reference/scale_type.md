Scale Types
===========

The `<scale-type>` options can be used to control the projection of data and to define
categorical scales. To control where ticks and labels are placed on a scale,
please see the [Scale Type](../scale-type) page. To control the formatting
of scale labels, please see the [Formatters](../formatters) page.


List of `<scale-type>` values
-----------------------------

### `linear`

A simple linear scale from `min` to `max`.

    (linear)

Example: [`charts-reference/scale_linear`](/examples/charts-reference/scale_linear):
[![scale_linear.svg](/examples/charts-reference/scale_linear.svg)](/examples/charts-reference/scale_linear)


---
### `logarithmic`

A logarithmic scale. Default base is 10.

    (log)
    (log <base>)
    (logarithmic)
    (logarithmic <base>)

Example: [`charts-reference/scale_logarithmic`](/examples/charts-reference/scale_logarithmic):
[![scale_logarithmic.svg](/examples/charts-reference/scale_logarithmic.svg)](/examples/charts-reference/scale_logarithmic)


---
### `categorical`

A categorical scale. The `<data-ref>` argument is mandatory and should contain
the list of valid categories.

    (categorical <data-ref>)

Example: [`charts-reference/scale_categorical`](/examples/charts-reference/scale_categorical):
[![scale_categorical.svg](/examples/charts-reference/scale_categorical.svg)](/examples/charts-reference/scale_categorical)

---
### `inverted`

A modifier that inverts the scale. Can be used in conjunction with other scale
types.

    (inverted)
    (invert)

Example: [`charts-reference/scale_inverted`](/examples/charts-reference/scale_inverted):
[![scale_inverted.svg](/examples/charts-reference/scale_inverted.svg)](/examples/charts-reference/scale_inverted)
