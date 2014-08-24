
FnordMetric allows you to visualize your data without ever leaving SQL. Turning
a query result into a chart is literally one line of code.


FnordMetric v2.0.0 Documentation
--------------------------------

If you're like me you are probably thinking something along the lines of "Does
the world really need yet another data visualization tool?" right now. Surely,
there already is gnuplot and about a bazillion javascript libraries that let you
draw pretty charts.

Most of the existing open source tools require you to provide the input data in
some specific, custom format. I reckon this is not ideal. It means you have to
write a heap of reptitive glue code (or sed incantations if that's your thing)
to mangle your input data into that format. If you run a lot of ad-hoc queries
you have to waste a significant amount of time on this boring legwork that could
much better be spent on interesting tasks.

FnordMetric aims to fix that by extending standard SQL; it allows you to express
queries that return charts rather than tables.

An example says more than a thousand words so here is how you generate a shiny
timeseries chart from a csv file using nothing else than plain old SQL.
   ...

And this plots a point chart with error bars directly from a MySQL database:
   ...


### Table of Contents

+ Getting Started
  + [Installing Fnordmetric](#)
  + [Getting Started](#)

+ User Interfaces
  + [Command line interface](#)
  + [Web interface](#)

Query Language
  Overview
  DrawingDrawing Charts
    DRAW Statement
    Axes

  Built-in Functions
  Syntax Reference


Drawing Charts
Defining Series
Data Sources
Time-based Aggregations
Example Queries

Data Sources
  MySQL
  CSV

FnordMetric is a framework for collecting and visualizing timeseries data. It enables
you to build beautiful real-time analytics dashboards within minutes.

### FnordMetric UI

FnordMetric UI is a HTML5 / JavaScript API that lets you plug real-time data and charts into any
webpage without writing a single line of code. It gives you maximum flexiblitiy as you have full
control over layout and style with HTML and CSS.

FnordMetric UI uses WebSockets to communicate with a backend server. There are two backend
implementations: FnordMetric Classic (ruby + redis) and FnordMetric Enterprise (JVM).

You can use FnordMetric UI as a white label solution to power your custom real-time analytics apps.

<a href="/documentation/ui_index">Getting Started with FnordMetric UI &rarr;</a>
<br /><br />


### FnordMetric Enterprise

FnordMetric Enterprise is a JVM based timeseries database which serves as a backend for FnordMetric
UI. It can handle thousands of gauges and years worth of historical data. FnordMetric Enterprise runs
stand-alone, i.e. it does not depend on a backend store like redis.

FnordMetric Enterprise features disk persistence, a HTTP, TCP and UDP API, native WebSockets support
and CSV/JSON Export. FnordMetric Enterprise can be used as a drop-in replacement for StatsD+Graphite
(it is a lot faster, see Benchmarks).

<a href="/documentation/enterprise_index">Getting Started with FnordMetric Enterprise &rarr;</a>
<br /><br />


### FnordMetric Classic

FnordMetric Classic is powered by ruby and redis. It offers a ruby DSL for processing data
streams and building beautiful web dashboards from a collection of pre-made UI widgets.

You can also use the FnordMetric UI HTML5 API to display the data collected with FnordMetric
Classic on another website or build highly customized / white label views.

<a href="/documentation/classic_index">Getting Started with FnordMetric Classic &rarr;</a>

