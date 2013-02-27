Widgets
-------

FnordMetric includes a collection of UI widgets to display the data /
content of gauges. There is a simple ruby DSL to use these widgets.

_Example: Render a timeseries line chart on dashboard "Sales"_

    widget 'Sales',
      :title => "Sales per Minute",
      :gauges => [:sales_per_minute],
      :type => :timeline,
      :width => 100



### TimelineWidget

The timeline widget renders a timeseries graph with simple date/time controls.
The available renderers are `line`, `area` and `flow`.

<img src="/img/widget_timeseries.png" width="600" />

Valid options:

<table>
  <tr>
    <th>type <i>(mandatory)</i></th>
    <td>
      The type of this widget. Must be `:timeline`.
    </td>
  </tr>
  <tr>
    <th>gauges <i>(mandatory)</i></th>
    <td>
      Array of symbols, each symbol is a gauge name to display. e.g. `[:gauge1, :gauge2]`
    </td>
  </tr>
  <tr>
    <th>title</th>
    <td>
      The title of this widget (default: no title).
    </td>
  </tr>
  <tr>
    <th>width</th>
    <td>
      The width of this widget in percent of the viewport (default: 100).
    </td>
  </tr>
  <tr>
    <th>autoupdate</th>
    <td>
      Refresh the contents of this widget every N seconds (default: none).
    </td>
  </tr>
</table>
<br />

### NumbersWidget

The numbers widget displays the formatted value of a gauge:

<img src="/img/widget_numbers.png" width="500" />

Valid options:

<table>
  <tr>
    <th>type <i>(mandatory)</i></th>
    <td>
      The type of this widget. Must be `:numbers`.
    </td>
  </tr>
  <tr>
    <th>gauges <i>(mandatory)</i></th>
    <td>
      Array of symbols, each symbol is a gauge name to display. e.g. `[:gauge1, :gauge2]`
    </td>
  </tr>
  <tr>
    <th>offsets <i>(mandatory)</i></th>
    <td>
      Array of integer offsets at which the gauges are displayed. If the gauges
      resolution is 1 minute, then `[1,5,10]` will display the gauges values 1, 5
      and 10 minutes ago.<br />There is a hack to display the sum of an offset.
      To display e.g. the sum of all values of the last 5 minutes, set this to `["s5"]`.</td>
  </tr>
  <tr>
    <th>title</th>
    <td>
      The title of this widget (default: no title).
    </td>
  </tr>
  <tr>
    <th>width</th>
    <td>
      The width of this widget in percent of the viewport (default: 100).
    </td>
  </tr>
  <tr>
    <th>autoupdate</th>
    <td>
      Refresh the contents of this widget every N seconds (default: none).
    </td>
  </tr>
</table>
<br />


### BarsWidget

This widgets renders a bar chart. It requires a three-dimensional gauge.

<img src="/img/widget_bars.png" width="600" />


Valid options:

<table>
  <tr>
    <th>type <i>(mandatory)</i></th>
    <td>
      the type of this widget. must be `:bars`
    </td>
  </tr>
  <tr>
    <th>gauges <i>(mandatory)</i></th>
    <td>
      array of symbols, each symbol is a gauge name to display. e.g. `[:gauge1, :gauge2]`. the gauges must to be three dimensional
    </td>
  </tr>
  <tr>
    <th>title</th>
    <td>
      The title of this widget (default: no title).
    </td>
  </tr>
  <tr>
    <th>width</th>
    <td>
      The width of this widget in percent of the viewport (default: 100).
    </td>
  </tr>
  <tr>
    <th>autoupdate</th>
    <td>
      Refresh the contents of this widget every N seconds (default: none).
    </td>
  </tr>
</table>
<br />

### ToplistWidget

This widget renders a toplist of the top X items. It requires a three dimensional gauge.

<img src="/img/widget_toplist.png" width="500" />

Valid options:

<table>
  <tr>
    <th>type <i>(mandatory)</i></th>
    <td>
      The type of this widget. Must be `:timeline`.
    </td>
  </tr>
  <tr>
    <th>gauges <i>(mandatory)</i></th>
    <td>
      Array of symbols, each symbol is a gauge name to display. e.g. `[:gauge1, :gauge2]`. The gauges must to be three dimensional.
    </td>
  </tr>
  <tr>
    <th>title</th>
    <td>
      The title of this widget (default: no title).
    </td>
  </tr>
  <tr>
    <th>width</th>
    <td>
      The width of this widget in percent of the viewport (default: 100).
    </td>
  </tr>
  <tr>
    <th>autoupdate</th>
    <td>
      Refresh the contents of this widget every N seconds (default: none).
    </td>
  </tr>
</table>
<br />

### HTMLWidget

This widget renders a snippet of static html. You can use this to e.g.
include external charts in a FnordMetric dashboard.

<table>
  <tr>
    <th>type <i>(mandatory)</i></th>
    <td>
      The type of this widget. Must be `:html`.
    </td>
  </tr>
  <tr>
    <th>html/th>
    <td>
      The html to render.
    </td>
  </tr>
</table>
<br />
