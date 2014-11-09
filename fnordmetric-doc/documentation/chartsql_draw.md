The DRAW Statement
==================

The DRAW statement allows you to specify that the query result should be
returned as a chart (or other visualization) rather than a table. The
general syntax of the DRAW statement is:

    DRAW <charttype> [ WITH ] <options>;

All SELECT statements that follow a DRAW statement are interpreted as chart data.
The currently implemented charttypes are `AREACHART`, `BARCHART`, `HEATMAP`, 
`HISTOGRAM`, `LINECHART` and `POINTCHART`;

For example, a simple draw statement to draw a line chart looks like this:

    DRAW LINECHART:
    SELECT x, y FROM table;


Adding Series
-------------

All charts render one or more series of data. Data is added to charts by
subsequent SELECT statements. Each SELECT statement's results are added to
the last DRAW statement that preceeds it.

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

Besides the four "magic" columns we alread discussed (`x`, `y`, `y` and `series`),
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

Axes
----

- specifying axes
   * titles
   * multiyple y axis

Configuring Value Domains
-------------------------

- specifying domains
   * logarithmic

Adding Legends
--------------

Adding a Chart Title/Subtitle
-----------------------------

- setting a title / subtitle

Combining Charts
----------------

- combining charts / overdraw



