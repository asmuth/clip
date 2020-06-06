# Using the `plot` module

The `plot` module is used to create two-dimensional plots from structured input
data, such as CSV or GeoJSON files. To use the plot module, add the following line
to an empty file:

    (class plot)

The remainder of the file should then contain a list of commands. Commands are
generally divided into two categories. Geometry commands, such as `lines`, `points`
or `bars` add shapes to the plot. Layout commands, such as `axis`, `grid` or `legend`
control the appearance of the chart layout.

### List of commands

This page lists all available commands for the `plot` module. Please follow the
respective links for more details on individual commands.

<pre><code><i>;; Geometry Commands</I>
(<a href="/plot/areas">areas</a> ...)
(<a href="/plot/bars">bars</a> ...)
(<a href="/plot/errorbars">errorbars</a> ...)
(<a href="/plot/labels">labels</a> ...)
(<a href="/plot/lines">lines</a> ...)
(<a href="/plot/points">points</a> ...)
(<a href="/plot/polygons">polygons</a> ...)
(<a href="/plot/rectangles">rectangles</a> ...)
(<a href="/plot/vectors">vectors</a> ...)

<i>;; Layout Commands</i>
(<a href="/plot/axes">axes</a> ...)
(<a href="/plot/axis">axis</a> ...)
(<a href="/plot/grid">grid</a> ...)
(<a href="/plot/legend">legend</a> ...)
</code></pre>


### Syntax Example

Here is a full example that will generate a simple line chart:

    (class plot)

    ;; add axes
    (axes)

    ;; add lines
    (lines
        data-x (100 300 500 700 900)
        data-y (200 300 200 300 100)
        color #06c
        stroke-width 2pt)
