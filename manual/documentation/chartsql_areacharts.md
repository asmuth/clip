Area Charts
===========

The syntax for areacharts is:

    DRAW AREACHART
        [ [ WITH ]
              common_chart_options
              [ STACKED ] ]


2D Area Charts
---------------

All 2D Area Chart series must have at least two columns: `x` and `y`. A 2D Area
Chart will draw one area between 0 to y for each x for for each input row.

If multiple series are specified and the STACKED keyword is set in the DRAW
AREACHART statement, the series areas will be stacked.

The type of the x column can be any of `String`, `DateTime`, `Numeric`. The type
of the y column must be `Numeric`.

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
      if set, each series will be displayed as a separate area
    </td>
  </tr>
  <tr>
    <th>linestyle <i>(optional)</i></th>
    <td>
      the line style, valid values are `solid` and `none`. default: `none`
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
      the area/line/point color (hex value, eg. #ff0000)
    </td>
  </tr>
</table>



3D Area Charts
---------------

All 3D Area Chart series must have at least two columns: `x`, `y` and `z`. A 3D
Area Chart will draw one area between y to z for each x for for each input row.

If multiple series are specified and the STACKED keyword is set in the DRAW
AREACHART statement, the series areas will be stacked.

The type of the x column can be any of `String`, `DateTime`, `Numeric`. The type
of the y and z column must be `Numeric`.

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
      the lower y bound of the area, type must be `numeric`
    </td>
  </tr>
  <tr>
    <th>z <i>(mandatory)</i></th>
    <td>
      the upper y bound of the area, type must be `numeric`
    </td>
  </tr>
  <tr>
    <th>series <i>(optional)</i></th>
    <td>
      if set, each series will be displayed as a separate area
    </td>
  </tr>
  <tr>
    <th>linestyle <i>(optional)</i></th>
    <td>
      the line style, valid values are `solid` and `none`. default: `none`
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
      the area/line/point color (hex value, eg. #ff0000)
    </td>
  </tr>
</table>



Examples
--------

#### 2D Area Chart:

<img src="/img/examples_area_with_points.png" width="800" />

    IMPORT TABLE example_data
       FROM 'csv:examples/data/areadata.csv?headers=true';

    DRAW AREACHART WITH
       AXIS TOP
       AXIS BOTTOM
       AXIS RIGHT
       AXIS LEFT
       YDOMAIN 0, 50;

    SELECT
          'data' AS series,
          x AS x,
          y AS y,
          'circle' as pointstyle,
          'solid' as linestyle,
          3 as pointsize,
          2 as linewidth
       FROM example_data
       WHERE series = "series1";

#### 3D Area Chart:

<img src="/img/examples_area_ranges.png" width="800" />

    IMPORT TABLE example_data
       FROM 'csv:examples/data/areadata2.csv?headers=true';

    DRAW AREACHART WITH
       AXIS TOP
       AXIS BOTTOM
       AXIS RIGHT
       AXIS LEFT;

    SELECT
          series AS series,
          x AS x,
          y AS y,
          z AS z
       FROM example_data
       WHERE series = "series2" or series = "series1";
