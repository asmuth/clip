Axes
====

## Ticks & Labels

The axis layout setting allows you to control how ticks and labels are placed
on the axis.

    axis-top: <axis-layout> <label-format>;
    axis-right: <axis-layout> <label-format>;
    axis-bottom: <axis-layout> <label-format>;
    axis-left: <axis-layout> <label-format>;

Alternatively, you can configure the layout and formater separately:

    axis-top-layout: <axis-layout>;
    axis-top-format: <label-format>;

### Tick Placement

The layout of an axis controls where how ticks an labels are placed. The list of
valid `<axis-layout>` values is:

---

#### `linear`
#### `linear(N)`

The `linear` layout will place a 'tick' every `N` units. If no `N` parameter
is provided it will be chosen automatically.

![axis_linear.svg](/documentation/figures/axis_linear.svg)

---

#### `linear(N, align A)`

The same as the `linear` layout, but the ticks will be aligned to an arbitrary
start value `A`.

![axis_linear_align.svg](/documentation/figures/axis_linear_aligned.svg)

---

#### `discrete`

The `discrete` layout marks a number of discrete groups on the axis. It
is usually used when plotting bar/column charts.

![axis_discrete.svg](/documentation/figures/axis_discrete.svg)

---

#### `subdivide`
#### `subdivide(N)`

The `subdivide` layout will divide the axis into N evenly sized divisions
and then mark each region boundary.

![axis_discrete.svg](/documentation/figures/axis_subdivide.svg)


### Label Formatter

The `<label-format>` setting controls how values are formatted for display. For
example, you can use it to set the floating point display precision, add a unit
of measurement to display values or display values as human-readable date strings.
Fore more information on setting the label label format, please refer to the
[Formatter](/documentation/formatter) page.
