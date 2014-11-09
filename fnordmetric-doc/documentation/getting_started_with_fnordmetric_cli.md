Getting Started with fnordmetric-cli
====================================

_This guide will walk you through executing a simple ChartSQL query from the
command line. If you do not have installed FnordMetric yet, read the
[Installation page](/documentation/installation) first._


As the first simple example we will create a line chart that displays the
average temperatures by city and month and looks like this:

<img src="/img/simple_linechart.png" width="800" />


Importing data from CSV
-----------------------

ChartSQL allows you to import tables from external data sources using the
IMPORT statement. For our example, assume we have a CSV file called city_temperatures.csv.
The contents of the city_temperatures.csv file look like this:

    city,month,temperature
    Tokyo,Dec,9.6
    New York,Nov,8.6
    ...

Download the city_temperatures.csv file and put it on a folder in your system.
In the same folder create another file called example_query.sql with this
content:

    IMPORT city_temperatures FROM "csv://city_temperatures.csv?headers=true"

    SELECT * FROM city_temperatures;

This will import the city_temperatures table with three columns `city`, `month`
and `temperature` from our csv file and return all rows from the table.

To execute the query we just wrote from the command line, we using the
fnordmetric-cli binary:

    $ fnordmetric-cli example_query.sql

This should print something like this:

    ==================================
    | city   | month  | temperature  |
    ==================================
    | Tokyo  | Dec    | 9.6          |
    | Tokyo  | Nov    | 13.9         |
    | Tokyo  | Oct    | 18.3         |
    | Tokyo  | Sep    | 23.3         |
    | ...    | ...    | ...          |


Visualizing SQL Query Results
------------------------------

Now lets create a simple line chart from the data. The ChartSQL DRAW statement
allows you to specify that the query result should be returned as a chart (or
other visualization) rather than a table.

All SELECT statements that follow a DRAW statement are interpreted as chart data.
and must return at least a `x` and a `y` result list column. In our example
we want to plot the month on the X axis and the temperature on the Y axis,
so we add this DRAW statement to our example_query.sql file:


    IMPORT city_temperatures FROM "csv://city_temperatures.csv?headers=true"

    DRAW LINECHART;

    SELECT city as series, month as X, temperature AS Y FROM city_temperatures;

Let's execute the query and render the chart to SVG:

    $ fnordmetric-cli --format example_query.sql > example_chart.svg

Open the example_chart.svg file in a browser. You'll see a chart that looks
somewhat like the chart in the beginning of this guide. We are still missing
axes, the legend and points. After we add these, we arrive at out final
example_query.sql file:

    IMPORT city_temperatures FROM "csv://city_temperatures.csv?headers=true"

    DRAW LINECHART WITH
        AXIS LEFT
        AXIS BOTTOM
        LEGEND TOP LEFT INSIDE;

    SELECT
        city as series,
        month as X,
        temperature AS Y,
        "circle" as pointstyle
    FROM city_temperatures;


You now have now seen how to execute a simple ChartSQL query from the command line,
but there is a lot more you can do. These are good docs to read next:

  + [Examples](/examples/)
  + [ChartSQL Query Language](/chartsql/introduction/)
  + [The DRAW Statement](/documentation/chartsql/draw_statement/)
  + [External Data Source](/documentation/chartsql/external_data_sources/)
  + [FnordMetric Server](/documentation/getting_started/fnordmetric-server/)
