Bar Charts
=========

The syntax for barcharts is:

    DRAW BARCHART
        [ [ WITH ]
              common_chart_options
              [ ORIENTATION { HORIZONTAL | VERTICAL } ]
              [ STACKED ] ]


2D Bar Charts
------------

All 2D Bar Chart series must have at least two columns: `x` and `y`. A 2D Bar
Chart will draw one bar for each x with a height of y.

If the ORIENTATION is set to VERTICAL, vertical bars (i.e. a column chart) will
be drawn, if the ORIENTATION is set to HORIZONTAL, horizontal bars (i.e. a bar
chart) will be drawn.

If multiple series are specified, the bars bars will be grouped by their series
and x values and each bar group will have a distinct color. If the STACKED keyword
is set in the DRAW BARCHART statement, all bars in a group will be stacked on top
of each other.

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
      the y axis value, type must be `Numeric`
    </td>
  </tr>
  <tr>
    <th>series <i>(optional)</i></th>
    <td>
      if set, each series will be displayed as a separate area
    </td>
  </tr>
  <tr>
    <th>label <i>(optional)</i></th>
    <td>
      a label to be displayed next to each bar
    </td>
  </tr>
  <tr>
    <th>color <i>(optional)</i></th>
    <td>
      the bar color (hex value, eg. #ff0000)
    </td>
  </tr>
</table>


3D Bar Charts
------------

All 3D Bar Chart series must have at least three columns: `x`, `y` and `z`. A 3D
Bar Chart will draw one bar for each x from y to z.

If the ORIENTATION is set to VERTICAL, vertical bars (i.e. a column chart) will
be drawn, if the ORIENTATION is set to HORIZONTAL, horizontal bars (i.e. a bar
chart) will be drawn.

If multiple series are specified, the bars bars will be grouped by their series
and x values and each bar group will have a distinct color. If the STACKED keyword
is set in the DRAW BARCHART statement, all bars in a group will be stacked on top
of each other.

The type of the x column can be any of `String`, `DateTime`, `Numeric`. The type
of the y and z columns must be `Numeric`.

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
      the lower bound of the bar, type must be `Numeric`
    </td>
  </tr>
  <tr>
    <th>z <i>(mandatory)</i></th>
    <td>
      the upper bound of the bar, type must be `Numeric`
    </td>
  </tr>
  <tr>
    <th>series <i>(optional)</i></th>
    <td>
      if set, each series will be displayed as a separate area
    </td>
  </tr>
  <tr>
    <th>label <i>(optional)</i></th>
    <td>
      a label to be displayed next to each bar
    </td>
  </tr>
  <tr>
    <th>color <i>(optional)</i></th>
    <td>
      the bar color (hex value, eg. #ff0000)
    </td>
  </tr>
</table>

Examples
--------

#### Horizontal 2D Bar Chart

<img src="/img/examples_horizontal_bars.png" width="800" />

    IMPORT TABLE gdp_per_country
       FROM 'csv:examples/data/gbp_per_country_simple.csv?headers=true';

    DRAW BARCHART WITH
        ORIENTATION HORIZONTAL
        LABELS
        AXIS LEFT
        AXIS BOTTOM
        YDOMAIN 0, 20;

    SELECT 'gdp per country' AS series, country AS x, gbp / 1000000 AS y, gbp / 1000 + " Billion $" as label
        FROM gdp_per_country
        LIMIT 10;

#### Horizontal 2D Bar Chart with Multiple Series

<img src="/img/examples_vertical_bars.png" width="800" />

    IMPORT TABLE gdp_per_capita
       FROM 'csv:examples/data/gdp_per_capita.csv?headers=true';

    DRAW BARCHART WITH
        ORIENTATION VERTICAL
        AXIS BOTTOM
        GRID HORIZONTAL
        LEGEND TOP RIGHT INSIDE;

    SELECT year AS series, isocode AS x, gdp AS y
        FROM gdp_per_capita
        WHERE year = "2010" OR year = '2009' or year = '2008'
        ORDER BY gdp DESC
        LIMIT 9;

#### Horizontal Stacked 2D Bar Chart

<img src="/img/examples_stacked_bars.png" width="800" />

    IMPORT TABLE gdp_per_capita
       FROM 'csv:examples/data/gdp_per_capita.csv?headers=true';

    DRAW BARCHART WITH
      ORIENTATION HORIZONTAL
      STACKED
      AXIS LEFT
      AXIS BOTTOM
      LEGEND BOTTOM RIGHT INSIDE;

    SELECT year AS series, isocode AS x, gdp AS y
        FROM gdp_per_capita
        WHERE year = "2010" OR year = '2009' or year = '2008'
        LIMIT 20 OFFSET 40;

#### Horizontal 3D Bar Chart

<img src="/img/examples_horizontal_ranges.png" width="800" />

    IMPORT TABLE example_data
       FROM 'csv:examples/data/areadata.csv?headers=true';

    DRAW BARCHART
        ORIENTATION HORIZONTAL
        AXIS BOTTOM
        AXIS LEFT;

    SELECT series as series, w as x, y as y, z as z from example_data WHERE series = "series1";
    SELECT series as series, w as x, y + 20 as y, z + 20 as z from example_data WHERE series = "series2";
