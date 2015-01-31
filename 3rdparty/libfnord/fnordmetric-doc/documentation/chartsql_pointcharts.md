Point Charts
===========

The syntax for pointcharts is:

    DRAW POINTCHART
        [ [ WITH ]
              common_chart_options ]


2D Point Charts
---------------

All 2D Point Chart series must have at least two columns: `x` and `y`. A 2D Point
Chart will draw one point at (x, y) for each input row.

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
      if set, each series will be displayed as a distinct collection of points
      (i.e. in a different color)
    </td>
  </tr>
  <tr>
    <th>pointstyle <i>(optional)</i></th>
    <td>
      the point style, valid values are `circle` and `none`. default: `none`
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

3D Point Charts
---------------

All 3D Point Chart series must have at least three columns: `x`, `y` and `z`.
A 3D Point Chart will draw one point at (x, y) with size z for each input row.

The type of the x column can be any of `String`, `DateTime`, `Numeric`.
The type of the y column can be any of  `String`, `DateTime`, `Numeric`. The
type of the z column must be `Numeric`.

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
    <th>z <i>(mandatory)</i></th>
    <td>
      the point size, type must be `Numeric`
    </td>
  </tr>
  <tr>
    <th>series <i>(optional)</i></th>
    <td>
      if set, each series will be displayed as a distinct collection of points
      (i.e. in a different color)
    </td>
  </tr>
  <tr>
    <th>pointstyle <i>(optional)</i></th>
    <td>
      the point style, valid values are `circle` and `none`. default: `none`
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

#### 2D Point Chart:

<img src="/img/examples_simple_scatter.png" width="800" />

    IMPORT TABLE example_data
       FROM 'csv:examples/data/point_example.csv?headers=true';

    DRAW POINTCHART WITH
       AXIS BOTTOM
       AXIS LEFT
       AXIS TOP
       AXIS RIGHT
       GRID HORIZONTAL VERTICAL;

    SELECT series AS series, x AS x, y AS y
       FROM example_data;


#### 3D Point Chart with Labels:

<img src="/img/examples_pointchart_with_labels.png" width="800" />

    IMPORT TABLE example_data
       FROM 'csv:examples/data/point_example.csv?headers=true';

    DRAW POINTCHART WITH
       AXIS BOTTOM
       AXIS LEFT
       AXIS TOP
       AXIS RIGHT
       YDOMAIN -40, 60
       LABELS;

    SELECT series AS series, x AS x, y AS y, z as z, z as label
       FROM example_data;
