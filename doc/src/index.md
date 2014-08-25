FnordMetric v2.0.0 Documentation
--------------------------------

FnordMetric allows you to visualize your data without ever leaving SQL. Turning
a query result into a chart is literally one line of code.

### Table of Contents

+ Getting Started
  + [What is FnordMetric?](#)
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


### What is FnordMetric?

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

An example says more than a thousand words so here is how you generate a shiny
timeseries chart from a csv file using nothing else than plain old SQL.
   ...

And this plots a point chart with error bars directly from a MySQL database:
   ...


