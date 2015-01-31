The DRAW Statement
==================

The DRAW statement allows you to specify that the query result should be
returned as a chart (or other visualization) rather than a table. The
general syntax of the DRAW statement is:

    DRAW <charttype> [ WITH ] <options>;

All SELECT statements that follow a DRAW statement are interpreted as chart data.
The currently implemented charttypes are `AREACHART`, `BARCHART`, `LINECHART`
and `POINTCHART`;

For example, a simple draw statement to draw a line chart looks like this:

    DRAW LINECHART:
    SELECT x, y FROM table;


Adding Series
-------------

All charts render one or more series of data. Data is added to charts by
subsequent SELECT statements. Each SELECT statement's results are added to
the last DRAW statement that precedes it.

    DRAW LINECHART;  -- first chart
    SELECT ...;      -- data is added to first chart
    SELECT ...;      -- data is added to first chart

    DRAW BARCHART;   -- second chart
    SELECT ...;      -- data is added to second chart

Each SELECT that succeeds a DRAW statement must have at least a `x` and a `y`
result list column. Some charts also accept a `z` column.

    SELECT ... as x, ... as y FROM ...;

You can set the series name by returning a `series` column:

    SELECT "myseries" as series, ... as x, ... as y FROM ...;

You can also use the `series` column to return multiple series in a single
SELECT statement:

    SELECT ... as series, ... as x, ... as y FROM ...;


For example, assume we have a table `city_temperatures` that has three columns
`month`, `temperature` and `city`. We could draw a line chart that plots
temperature on the Y axis and the month on the X axis and draws one line per
city like this:

    DRAW LINECHART;

    SELECT city AS series, month AS x, temperature AS y
        FROM city_temperatures;


[See more examples of queries with DRAW statements](/examples).


Labels, Colors & more
---------------------

Besides the four "magic" columns we already discussed (`x`, `y`, `y` and `series`),
the SELECT result list may contain these optional columns:

#### label

When the result list has a label column, the value of the label column is attached
to each point as a visible "tag" or "tooltip". The values in the label column
may be of any type that is convertible to string.

    SELECT ... as x, ... as y, "value is: " + value as label FROM ...;


#### color

Use the color column to control the color of the series (i.e. line/area/points).
The values in the color column must be strings and either one of the predefined
colors (`color0`...`color9`) or a hex rgb color value. For Example

    SELECT ... as x, ... as y, "color3" as color FROM ...;

    SELECT ... as x, ... as y, "#f73e14" as color FROM ...;

#### point_style

Use the point_style column to control the point style of the series. Valid values
for the point_style column are `circle` and `none`. The point_style column is only
respected by the areachart, linechart and pointchart.

    SELECT ... as x, ... as y, "circle" as point_style FROM ...;

#### point_size

Use the point_size column to control the point size of the series. Valid values
for the point_size column are (0..100). The point_size column is only respected
by the areachart, linechart and pointchart.

    SELECT ... as x, ... as y, 4 as point_size FROM ...;

#### line_style

Use the line_style column to control the line style of the series. Valid values
for the line_style column are `none` and `solid`. The line_style column is only
respected by the linechart and areachart.

    SELECT ... as x, ... as y, "solid" as line_style FROM ...;

#### line_width

Use the line_width column to control the line width of the series. Valid values
for the line_width column are (0..100). The line_width column is only
respected by the linechart and areachart.

    SELECT ... as x, ... as y, "2" as line_width FROM ...;

[See the point and line styles reference](/documentation/chartsql/point_and_line_styles)

Drawing Timeseries Data
-----------------------

If the values returned in the `x` or `y` result list columns are of the
DateTime type, the chart will be drawn as a timseries chart. For example:


    SELECT FROM_TIMESTAMP(my_timestamp) AS x, my_value AS y FROM ...


Axes
----

A chart has no axes by default. You must explicitly add all axes. The syntax
to add axes is:

    DRAW <charttype> [ WITH ]
        [ AXIS { TOP | RIGHT | BOTTOM | LEFT }
            [ TITLE 'title' ]
            [ TICKS [ { INSIDE | OUTSIDE | OFF } ] [ ROTATE deg ]]...;

For example, to add a bottom (X) and a left (Y) axis to a line chart:

    DRAW LINECHART WITH
        AXIS LEFFT
        AXIS BOTTOM;


#### Axis Title

You can optionally specify an axis title that will be plotted next to the
axis:

    DRAW LINECHART WITH
        AXIS LEFFT TITLE "my y axis"
        AXIS BOTTOM TITLE "my x axis";

#### Labels Positions & Rotated Labels

You can also control the position of the axis labels and rotate them:

    DRAW LINECHART WITH
        AXIS LEFFT TICKS INSIDE
        AXIS BOTTOM TICKS OUTSIDE ROTATE 45;


Value Domains
-------------

By default, the chart will fit the X, Y and if applicable Z domains to include
all values. However, you can specify explicit domains. The syntax to specify
domains is:

    DRAW <charttype> [ WITH ]
        [ XDOMAIN [ min, max ] [ LOGARITHMIC ] [ INVERT ] ]
        [ YDOMAIN [ min, max ] [ LOGARITHMIC ] [ INVERT ] ]
        [ ZDOMAIN [ min, max ] [ LOGARITHMIC ] [ INVERT ] ];

For example, to draw a line chart with an X axis from 0 to 100 and a Y axis
from 0 to 10:

    DRAW LINECHART WITH
        XDOMAIN 0, 100
        YDOMAIN 0, 100
        AXIS BOTTOM
        AXIS LEFT;

#### Logarithmic Domains

You can also specify that a domain should be plotted logarithmically. For example:

    -- draw a log/log chart
    DRAW LINECHART WITH
        XDOMAIN LOGARITHMIC
        YDOMAIN 0, 100 LOGARITHMIC
        AXIS BOTTOM
        AXIS LEFT;

#### Inverted Domains

To invert/mirror a domain, use the INVERT keyword. For example:

    DRAW LINECHART WITH
        YDOMAIN INVERT
        AXIS BOTTOM
        AXIS LEFT;



Adding Legends
--------------

You can add one legend per DRAW statement. The syntax to add a legend is:

    DRAW <charttype> [ WITH ]
        LEGEND { TOP | BOTTOM } { LEFT | RIGHT} { INSIDE | OUTSIDE }
            [ TITLE 'title' ]

Some examples:

    -- draw a line chart with a legend outside of the chart on the
    -- bottom left corner:
    DRAW LINECHART WITH
        LEGEND BOTTOM LEFT OUTSIDE;

    -- draw a line chart with a legend inside/overlayed onto the chart
    -- on the top right corner with the title "mylegend":
    DRAW LINECHART WITH
        LEGEND TOP RIGHT INSIDE TITLE "mylegend";


Adding a Chart Title/Subtitle
-----------------------------

You can optionally specify a title and/or a subtitle for each DRAW statement.
The (sub)title will be plotted onto the chart.

    DRAW <charttype> [ WITH ]
        [ TITLE 'title' ]
        [ SUBTITLE 'subtitle' ]

An example:

    DRAW LINECHART WITH
        TITLE "mychart"
        SUBTITLE "mysubtitle";

Combining Charts
----------------

Successive DRAW statements will be drawn onto the same canvas. You can use this
draw statements to combine multiple chart types or draw data with multiple Y
axes:

#### Multiple Y Axes

Draw a line chart with two independent Y axes using two DRAW statements:

    DRAW LINECHART WITH
        AXIS LEFT TITLE "y axis 1";

    SELECT ....;

    DRAW LINECHART WITH
        AXIS RIGHT TITLE "y axis 2";

    SELECT ....;


[See an example for multiple Y axes](/examples/linecharts/multiple_y_axes/)

#### Combining Chart Types

Draw a line chart and bar chart over each other:

    DRAW LINECHART;
    SELECT ....;

    DRAW BARCHART;
    SELECT ....;


[See an example for a combined type chart](/examples/combined/barsandlines/)

