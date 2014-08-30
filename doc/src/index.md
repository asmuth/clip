FnordMetric v2.0.0 Documentation
================================

FnordMetric allows you to visualize your data without ever leaving SQL. Turning
a query result into a chart is literally one line of code.

Table of Contents
-----------------

+ Introduction
  + [Rationale](#rationale)
  + [Installing Fnordmetric](#installing-fnordmetric)
  + [Getting Started](#getting-started)

+ Query Language
  + [Overview](#)
  + [Extensions to standard SQL](#)
  + [Virtual Tables](#)
  + [Visualizing Results](#)
  + [Built-in Functions](#)
  + [Syntax Reference](#)

+ User Interfaces
  + [Command line interface](#)
  + [Web interface](#)

+ Chart Styles
  + [Line Charts](#)
  + [Area Charts](#)
  + [Bar Charts](#)
  + [Point/Bubble/Scatter Charts](#)
  + [Axis, Legend, Point and Line styles](#)
  + [Custom themes with CSS](#)
  + [Interactive Charts](#)

+ Data Sources
  + [MySQL Database](#)
  + [CSV Files](#)

+ Building Dashboards
  + [Getting started with dashboards](#)
  + [Dynamic dashboards](#)
  + [HTML5 API reference](#)

+ Embedding FnordMetric
  + [C++](#)
  + [C](#)
  + [Ruby](#)
  + [PHP](#)
  + [Python](#)

+ Appendix
  + [Contributing](#)
  + [Old Documentation](#)
  + [License](#)


Introduction
============

FnordMetric is a tool that lets you to:

  - Analyze data from different sources using a SQL query language
  - Rapidly create visualizations from SQL query results using a web UI or the
    command line interface
  - Plug these visualizations into any webpage to build fancy dashboards

FnordMetric can read its data from a number of sources:

  - A MySQL Database
  - A CSV File
  - An agent/sensor library that you can link into your binaries to export
    data directly from your code
  - A statsd compatible datasource

There are three ways to use FnordMetric.
  + Execute queries and build charts from the command line
  + web interface
  + language libraries

An example says more than a thousand words so here is how you generate a shiny
timeseries chart from a csv file using nothing else than plain old SQL.
   ...

And this plots a point chart with error bars directly from a MySQL database:
   ...


Rationale
---------

If you're like me you are probably thinking something along the lines of "Does
the world really need yet another data visualization tool?" right now. Surely,
there already is gnuplot and about a bazillion javascript libraries that let you
draw pretty charts.

However, Most of the existing open source tools require you to provide the input
data in
some specific, custom format. I reckon this is not ideal; it means you have to
write a heap of reptitive glue code (or sed incantations if that's your thing)
to mangle your input data into that format. If you run a lot of ad-hoc queries
you have to waste a significant amount of time on this boring legwork that could
much better be spent on interesting tasks.

FnordMetric aims to fix that by extending standard SQL; it allows you to express
queries that return charts rather than tables.


Installing Fnordmetric
----------------------

### Debian: via apt-get
### Ubuntu: via dpkg
### Mac OSX: via homebrew
### Windows
### Compile From Source


Getting Started
---------------

This chapter is for impatient people who don't like reading documentation. For more in-depth information you are kindly referred to the following chapters.

As the first simple example we will create a bar chart that displays gross
domestic product per country and looks like this:

     #
     #       #
     #       #      #
     #       #      #
     USA     UK     IRAN ...
    -----------------------------------------

Let's say we have a csv file containing the data that looks like this:

    country,gdp
    iran,8000
    uk,9000
    usa,1000

We can create the chart pictured above with this sql query"

    -- import the csv file as a table
    IMPORT gdp_per_country FROM "gdp_per_country.csv";

    -- draw a simple bar chart
    BEGIN BAR CHART;

    -- draw gdp of the top 10 countries
    CREATE SERIES WITH
      SELECT "gdp per country" as name gbp as x, country as y FROM gdp_per_country ORDER BY gdp DESC LIMIT 10;


Now lets add a custom style and axis deinitions:

  -- draw a simple bar chart
  DRAW BAR CHART
    WITH STYLE ...;

  -- draw gdp of the top 10 countries
  SERIES "gdp per country" FROM
    SELECT gbp, country FROM gdp_per_country ORDER BY gdp DESC LIMIT 10;
    WITH gdp AS X and country as Y;


Another example:

Draw a few latency distributions for multiple loadtest experiments sourced from
multiple csv files. The csv files contain two columns "time" and "runtime" and
one line for each request in the load test. The "time" column contains the wall
clock time at which the request was issued and the "runtime" column contains the
runtime of the request.

We will draw a single line graph that plots requests per minute vs the 90th
percentile latency in that time window.

    IMPORT experiment1 FROM "experiment_1.csv";

    DRAW LINE CHART;

    SERIES "experiment1" FROM
      SELECT count(*) as qps, nth_percentile(runtime, 90) as latency
        FROM experiment1
        GROUP BY time_window(time, 1s);

Lets add another experiment as a second series in the chart:

    IMPORT experiment1 FROM "experiment_1.csv";
    IMPORT experiment2 FROM "experiment_2.csv";

    BEGIN LINES;

    CREATE SERIES WITH SELECT
      "experiment1" as title, count(*) as x, nth_percentile(runtime, 90) as y
      FROM experiment1
      GROUP BY time_window(time, 1s);

    SERIES "experiment2" FROM
      SELECT count(*) as qps, nth_percentile(runtime, 90) as latency
        FROM experiment2
        GROUP BY time_window(time, 1s);


To make that last query more interesting lets say we put all our test results
into a single file with an additional "experiment" columns and try to recreate
the same chart:

    -- load a csv file with per request logs from a load test
    IMPORT loadtests FROM "myfile.csv";

    -- draw a graph of qps vs 90thpercentile, one series for each experiment
    BEGIN CHART;

    DRAW LINES experiment FROM
      SELECT experiment, count(*) as qps, nth_percentile(runtime) as latency
        FROM loadtests
        GROUP BY experiment, time_window(time, 1s);




Query Language
==============

The FnordMetric query language is a superset of SQL99 (SELECT statements only).
It supports all the operations you would expect in your standard SQL database
like GROUP BY or HAVING statements and joins. Additionally, FnordMetric implements
a number of new statements to create charts directly from SQL.

Extensions to standard SQL
--------------------------

### GROUP BY and HAVING extensions

Like MySQL, fnordmetric SQL extends the use of GROUP BY so that the select list
can refer to nonaggregated columns not named in the GROUP BY clause. If you use
a group function in a statement containing no GROUP BY clause, it equivalent to
grouping on all rows. This assumes that the nongrouped columns will have the same
group-wise values. Otherwise, the result is undefined. The same applies for
the HAVING clause.


### GROUP BY and HAVING extensions

Like MySQL, fnordmetric SQL extends the use of GROUP BY so that the select list
can refer to nonaggregated columns not named in the GROUP BY clause. If you use
a group function in a statement containing no GROUP BY clause, it equivalent to
grouping on all rows. This assumes that the nongrouped columns will have the same
group-wise values. Otherwise, the result is undefined. The same applies for
the HAVING clause.


### GROUP BY and HAVING extensions

Like MySQL, fnordmetric SQL extends the use of GROUP BY so that the select list
can refer to nonaggregated columns not named in the GROUP BY clause. If you use
a group function in a statement containing no GROUP BY clause, it equivalent to
grouping on all rows. This assumes that the nongrouped columns will have the same
group-wise values. Otherwise, the result is undefined. The same applies for
the HAVING clause.


Syntax Reference
----------------

### IMPORT statement

    IMPORT TABLE tablename FROM CSV { 'filename' | STDIN }
        [ [ WITH ]
              [ DELIMITER [ AS ] 'delimiter' ]
              [ ROW_DELIMITER [ AS ] 'row_delimiter' ]
              [ HEADER ]
              [ QUOTE [ AS ] 'escape' ]
              [ ESCAPE [ AS ] 'escape' ] ]

### DRAW ... CHART statement

    DRAW BAR CHART
        [ [ WITH ] ]
              [ TITLE 'title' ]
              [ SUBTITLE 'subtitle' ]
              [ ORIENTATION {HORIZONTAL|VERTICAL} ]
              [ STACKED {TRUE|FALSE} ]
              [ LABELS {INSIDE|OUTSIDE|OFF} ] ]


### DRAW AXIS statement

    DRAW AXIS {TOP|RIGHT|BOTTOM|LEFT}
        [ [ WITH ] ]
              [ TITLE 'title' ]
              [ DOMAIN min, max [ LOGARITHMIC ] ]
              [ TICKS { INSIDE | OUTSIDE | OFF } ];

### SELECT statement

    SELECT
        [ALL | DISTINCT | DISTINCTROW ]
        select_expr [, select_expr ...]
        [FROM table_references
        [WHERE where_condition]
        [GROUP BY {col_name | expr | position}
          [ASC | DESC], ...]
        [HAVING where_condition]
        [ORDER BY {col_name | expr | position}
          [ASC | DESC], ...]
        [LIMIT {[offset,] row_count | row_count OFFSET offset}];



Hacking
=======

Contributing
------------

To contribute, please fork this repository, make your changes and run the 
specs, commit them to your github repository and send me a pull request.
Need help, head on over to our [Google Groups][1] page to discuss any ideas
that you might have.

### Ways to contribute to FnordMetric:

  + report bugs or suggest new features
  + suggest or implement new options for existing charts or new chart types
  + improve the documentation
  + publish howtos, tutorials and videos
  + add a new default stylesheet (requires css knowledge)
  + add a new language binding (requires minimal c++ knowledge)
  + add a new query backend (requires c++ knowledge)
  + add a new render target (requires c++ knowledge)
  + add some new features to the query engine (requires c++ knowledge)
  + many more! improve all the things :)

### Other Open Source Software included in FnordMetric

  + Christian Parpart's xzero -- HTTP library (http://github.com/xzero)
  + Marijn Haverbeke's CodeMirror --- javascript code editor (https://github.com/marijnh/codemirror)

### Individial Contributors to FnordMetric Version 2.0.0

  + Christian Parpart (http://github.com/trapni)

### Individial Contributors to FnordMetric Version 0.0.8 - v1.2.9

 + Simon Menke (http://github.com/fd),
 + Bruno Michel (http://github.com/nono),
 + Marco Borromeo (http://github.com/mborromeo),
 + Leo Lou (http://github.com/l4u)
 + Andy Lindeman (http://github.com/alindeman),
 + Jurriaan Pruis (http://github.com/jurriaan),
 + Kacper Bielecki (http://github.com/kazjote),
 + John Murray (http://github.com/JohnMurray),
 + Lars Gierth (http://github.com/lgierth),
 + Ross Kaffenberger (http://github.com/rossta),
 + Kunal Modi (http://github.com/kunalmodi),
 + Michael Fairchild (http://github.com/fairchild),
 + James Cox (http://github.com/imajes),
 + Pieter Noordhuis (http://github.com/pietern),
 + Tadas Ščerbinskas (http://github.com/tadassce),
 + Sebastian Korfmann (http://github.com/skorfmann)


License
-------

Copyright (c) 2011-2014 Paul Asmuth

FnordMetric is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FnordMetric is distributed in the hope that it will be useful,but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FnordMetric. If not, see <http://www.gnu.org/licenses/>.


