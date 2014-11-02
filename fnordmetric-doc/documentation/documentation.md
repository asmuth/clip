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
  + [Extensions to standard SQL](#)
  + [Virtual Tables](#)
  + [The DRAW statement](#)
  + [Built-in Functions](#)
  + [Syntax Reference](#)

+ User Interfaces
  + [Command line interface](#)
  + [Web interface](#)

+ Charts
  + [Line Charts](#)
  + [Bar Charts](#)
  + [Area Charts](#)
  + [Bubble Charts](#)
  + [Scatter Charts](#)
  + [Histograms](#)
  + [Heatmaps](#)
  + [Pie Charts](#)
  + [Boxplots](#)
  + [Error Bars](#)
  + [Gantt charts](#)
  + [Sparklines](#)

+ Customizing Charts
  + [Point and Line styles](#)
  + [Axis Styles](#)
  + [Legend Styles](#)
  + [Custom themes with CSS](#)
  + [Interactive Charts](#)

+ Data Sources
  + [MySQL Database](#)
  + [CSV Files](#)

+ Embedding Charts into the web
  + [Getting started with the HTML5 API](#)
  + [Building web dashboards](#)
  + [HTML5 API reference](#)

+ Embedding FnordMetric
  + [C++ API Reference](#)
  + [C API Reference](#)
  + [Ruby API Reference](#)
  + [PHP API Reference](#)
  + [Python API Reference](#)

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




