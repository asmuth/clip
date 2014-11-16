Getting Started with ChartSQL
=============================

_This guide will walk you through the required steps of executing a simple ChartSQL query 
from the command line. If you haven't installed FnordMetric yet, start by reading the
[Installation page](/documentation/installation) ._


For our first simple example we will create a line chart that displays the
average temperatures of multiple cities by month resulting in the following output:

<img src="/img/simple_linechart.png" width="800" />


Importing data from CSV
-----------------------

ChartSQL allows you to import tables from external data sources using the
IMPORT statement. Assume we have a CSV file called city_temperatures.csv 
where the content looks similar to this:

    city,month,temperature
    Tokyo,Dec,9.6
    New York,Nov,8.6
    ...

Download city_temperatures.csv to a folder on your system. Within the same folder create another file called example_query.sql with the following content:

    IMPORT TABLE city_temperatures FROM "csv://city_temperatures.csv?headers=true"

    SELECT * FROM city_temperatures;

This will import the table `city_temperatures` with three columns `city`, `month`
and `temperature` from our csv file and return each of its rows.

In order to execute the query we just wrote from the command line, we use the
fnordmetric-cli binary:

    $ fnordmetric-cli example_query.sql

Doing so should give an output similar to the following:

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

Now lets create a simple line chart from the data. ChartSQL's DRAW statement
allows you to specify that the query result should be returned as a chart (or
other visualization) rather than a table.

All SELECT statements that follow a DRAW statement are interpreted as chart data.
and must return at least a `x` and a `y` result list column. In our example
we want to plot the months on the X axis and the corresponding temperatures on the Y axis,
so we add this DRAW statement to our example_query.sql file:


    IMPORT TABLE city_temperatures FROM "csv://city_temperatures.csv?headers=true"

    DRAW LINECHART;

    SELECT city as series, month as X, temperature AS Y FROM city_temperatures;

Let's execute the query and render the chart to SVG:

    $ fnordmetric-cli --format svg example_query.sql > example_chart.svg

Now open example_chart.svg in a browser. You'll see a chart that looks
somewhat like the chart in the beginning of this guide. We are still missing
axes, the legend and points. After adding these, we have completed our
example_query.sql file:

    IMPORT TABLE city_temperatures FROM "csv://city_temperatures.csv?headers=true"

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


You are now able to execute a simple ChartSQL query from the command line,
but there is a lot more to discover! These are some good starting points for further reading:

  + [Examples](/examples/)
  + [The ChartSQL Query Language](/documentation/chartsql/introduction/)
  + [Getting Started with FnordMetric Server](/documentation/getting_started/fnordmetric-server/)
