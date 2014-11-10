Getting Started with FnordMetric
================================

### Getting Started in 5 minutes

We have three 5 minute getting started guides for impatient people who don't like
reading documentation (like the author). For more in-depth information you are
kindly referred to the remaining pages.

If you want to learn how to draw charts from SQL query results using ChartSQL and
the FnordMetric command line interface, read:

  + [Gettting Started with ChartSQL in 5 minutes](/documentation/getting_started/fnordmetric-cli)

If you want to see how you can collect timeseries data with FnordMetric Server
and query it using a web interface and ChartSQL, read:

  + [Gettting Started with FnordMetric Server in 5 minutes](/documentation/getting_started/fnordmetric-server)

---

What is FnordMetric?
====================

FnordMetric is a framework for visualizing and collecting (timeseries) data
using SQL. It extends standard SQL with ChartSQL, allowing you to write queries
that return charts rather than tables. The query results are rendered as SVG
vector graphics or images. [Check out some examples](/examples)

The charts can easily be embedded into any website and customized using CSS.
This enables you to build beautiful dashboards within minutes using nothing
else than SQL.

FnordMetric Binaries
--------------------

FnordMetric ships two binaries, each with it's own usecase:

#### FnordMetric CLI

fnordmetric-cli is probably best described as gnuplot meets SQL. It allows you
to run ChartSQL queries from the command line against external data sources,
like MySQL databases or CSV files.

Use fnordmetric-cli if you have already collected the source data and just
want to visualize it using ChartSQL.

[Getting started with fnordmetric-cli](/documentation/getting_started/fnordmetric-cli)
<br style="line-height:30px;"/>

#### FnordMetric Server

fnordmetric-server is a standalone HTTP server application. It exposes a web UI
and a HTTP API to run ChartSQL queries. The query results are returned as JSON,
SVG or PNG.

Like fnordmetric-cli, the server allows you to execute queries against a number
of external backends like MySQL databases or CSV files. Additionally, the server
includes a facility to collect timeseries data and store them either on local
disk or in external storage (MySQL, HBase).

You can use fnordmetric-server as a one-stop solution for metric collection and
charting. Since fnordmetric-server aims to be a StatsD+graphite competitor, it
implements a wire compatible StatsD API.

[Getting started with fnordmetric-server](/documentation/getting_started/fnordmetric-server)



FnordMetric Clients
-------------------

Additionally, there are a number of client libraries for FnordMetric:


#### FnordMetric for HTML5

Plug fnordmetric charts into any website and build beautiful dashboards within
minutes using only SQL and HTML/CSS. Includes generic dashboard control elements.

[Getting started with the HTML5 API](/documentation/getting_started/fnordmetric-server)

#### FnordMetric for JavaScript and node.js

The JavaScript client allows you to plug fnordmetric charts into any web
application. It also includes helper code to collect counters/timeseries
data in your node.js backend and send them to fnordmetric-server.

[Getting started with the JavaScript API](/documentation/getting_started/fnordmetric-server)

#### FnordMetric for Ruby (on Rails)

The ruby client allows you to plug fnordmetric charts into an Ruby (on Rails)
web application. It also includes helper classes to collect counters/timeseries
data and send them to fnordmetric-server.

[Getting started with the ruby API](/documentation/getting_started/fnordmetric-server)


FnordMetric Agents
------------------

FnordMetric also includes a number of pre-built agents to collect data from common sources:

  + Docker Agent (fm-docker-agent)



Rationale
---------

If you're like the author you are probably thinking something along the lines of "Does
the world really need yet another data visualization tool?" right now. Surely,
there already is gnuplot and about a bazillion javascript libraries that let
you draw pretty charts.

However, most of the existing open source tools require you to provide the input
data in some specific, custom format. I reckon this is not ideal. It means you
have to write a heap of reptitive glue code (or sed incantations if that's your
thing) to mangle your input data into that format. If you run a lot of ad-hoc
queries you have to waste a significant amount of time on this boring legwork
that could much better be spent on interesting tasks.

FnordMetric aims to fix that by extending standard SQL; it allows you to express
the data query and the chart specification in a coherent fashion (SQL).
