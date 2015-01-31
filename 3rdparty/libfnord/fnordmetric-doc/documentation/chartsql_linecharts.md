Line Charts
===========

The syntax for linecharts is:

    DRAW LINECHART
        [ [ WITH ]
              common_chart_options ]

2D Line Charts
--------------

All 2D Line Chart series must have at least two columns: `x` and `y`. A 2D Line
Chart will draw one line connecting all (x, y) points for each series. 

The type of the x column can be any of `String`, `DateTime`, `Numeric`. The type
of the y column can be any of  `String`, `DateTime`, `Numeric`.

#### Parameters:

<table>
  <tr>
    <th>x <i>(mandatory)</i></th>
    <td>
      the x axis value, type can be any of `String`, `DateTime`, `Numeric`
    </td>
  </tr>
  <tr>
    <th>y <i>(mandatory)</i></th>
    <td>
      the y axis value, type can be any of `String`, `DateTime`, `Numeric`
    </td>
  </tr>
  <tr>
    <th>series <i>(optional)</i></th>
    <td>
      if set, each series will be displayed as a separate line
    </td>
  </tr>
  <tr>
    <th>linestyle <i>(optional)</i></th>
    <td>
      the line style, valid values are `solid` and `none`. default: `solid`
    </td>
  </tr>
  <tr>
    <th>linewidth <i>(optional)</i></th>
    <td>
      the line width in pixel. default: 2
    </td>
  </tr>
  <tr>
    <th>pointstyle <i>(optional)</i></th>
    <td>
      the point style, valid values are `circle` and `none`. default: `none`.
    </td>
  </tr>
  <tr>
    <th>pointsize <i>(optional)</i></th>
    <td>
      the line width in pixel. default: 4
    </td>
  </tr>
  <tr>
    <th>label <i>(optional)</i></th>
    <td>
      a label to be displayed above each point
    </td>
  </tr>
  <tr>
    <th>color <i>(optional)</i></th>
    <td>
      the line/point color (hex value, eg. #ff0000)
    </td>
  </tr>
</table>

Examples
--------

#### Multi-Series Line Chart:

<img src="/img/examples_lines_with_points.png" width="800" />

    IMPORT TABLE city_temperatures
       FROM 'csv:examples/data/city_temperatures.csv?headers=true';

    DRAW LINECHART WITH
       AXIS LEFT
       AXIS BOTTOM
       LEGEND TOP LEFT INSIDE;

    SELECT
       city AS series,
       month AS x,
       temperature AS y,
       "Avg. Temperatures in " + month + " in " + city + ": " + temperature as label,
       "circle" as pointstyle
       FROM city_temperatures;


#### Lines with labels:

<img src="/img/examples_lines_with_labels.png" width="800" />

    IMPORT TABLE city_temperatures
       FROM 'csv:examples/data/city_temperatures.csv?headers=true';

    DRAW LINECHART WITH
       AXIS LEFT
       AXIS BOTTOM
       LABELS
       YDOMAIN -10, 40
       LEGEND TOP LEFT INSIDE;

    SELECT city AS series, month AS x, temperature AS y, temperature + "°C" as label, "circle" as pointstyle
       FROM city_temperatures
       WHERE city = "Berlin";
