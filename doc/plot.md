The `figure/plot` command
=========================

The `figure/plot` command is used to read input data from CSV or GeoJSON files
and draw a plot from it. The layout and styling of the plot is fully customizable
using the options described below.

Syntax
------

<pre><code>(figure/plot
    <a href="/plot/areas">areas</a> (...)
    <a href="/plot/axes">axes</a> (...)
    <a href="/plot/axis">axis</a> (...)
    <a href="/plot/bars">bars</a> (...)
    <a href="/plot/errorbars">errorbars</a> (...)
    <a href="/plot/grid">grid</a> (...)
    <a href="/plot/labels">labels</a> (...)
    <a href="/plot/legend">legend</a> (...)
    <a href="/plot/lines">lines</a> (...)
    <a href="/plot/points">points</a> (...)
    <a href="/plot/vectors">vectors</a> (...))
</code></pre>


List of elements
-------------------------

<table>
  <tbody>
    <tr>
      <td><a href="/plot/areas">areas</a></td>
      <td>Read input data and render it as an "areachart"</td>
    </tr>

    <tr>
      <td><a href="/plot/bars">bars</a></td>
      <td>Read input data and render it as an "barchart"</td>
    </tr>

    <tr>
      <td><a href="/plot/errorbars">errorbars</a></td>
      <td>Read input data and render it as an "errobars"</td>
    </tr>

    <tr>
      <td><a href="/plot/labels">labels</a></td>
      <td>Read input data and render it as text labels</td>
    </tr>

    <tr>
      <td><a href="/plot/lines">lines</a></td>
      <td>Read input data and render it as lines</td>
    </tr>

    <tr>
      <td><a href="/plot/points">points</a></td>
      <td>Read input data and render it as a point/scatter plot</td>
    </tr>

    <tr>
      <td><a href="/plot/rectangles">rectangles</a></td>
      <td>Read input data and render it as rectangles</td>
    </tr>

    <tr>
      <td><a href="/plot/vectors">vectors</a></td>
      <td>Read input data and render it as arrows</td>
    </tr>

  </tbody>
</table>


List of layout elements
-------------------------

<table>
  <tbody>

    <tr>
      <td><a href="/plot/axes">axes</a></td>
      <td>Add axes to the plot</td>
    </tr>

    <tr>
      <td><a href="/plot/axis">axis</a></td>
      <td>Add a single axis to the plot</td>
    </tr>

    <tr>
      <td><a href="/plot/labels">legend</a></td>
      <td>Add a legend to the plot</td>
    </tr>

    <tr>
      <td><a href="/plot/grid">grid</a></td>
      <td>Add background grid lines to the plot</td>
    </tr>

  </tbody>
</table>

List of options
---------------

<ul>
  <li><a href="/plot/color-maps">Color Maps</a></li>
  <li><a href="/plot/number-format">Number Formatters</a></li>
  <li><a href="/plot/scale-types">Scale Types</a></li>
  <li><a href="/plot/scale-layout">Scale Layout</a></li>
</ul>
