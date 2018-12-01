# PlotFX v0.1.0

PlotFX is a collection of lightweight tools for real-time metrics collection
and visualization with SQL. The tools allow you to record measurements and
build beautiful real-time dashboards within minutes. All using your favorite SQL
database (currently supported are MySQL, Postgres and EventQL).

<ul>
  <li>
    <h4 style="margin-bottom:.2em; font-size:16px;"><a href="/documentation/metric-collectd">metric-collectd</a></h4>
    <p>
      metric-collectd is a lightweight daemon that collects measurements and other
      timeseries data, pre-aggregates it and stores the resulting tuples into a
      backend database. metric-collectd can currently listen for samples in
      Text/StatsD or JSON format via UDP or HTTP. It can also pull samples via
      HTTP or SNMP.
    </p>
  </li>

  <li>
    <h4 style="margin-bottom:.2em; font-size:16px;"><a href="/documentation/metric-queryd">metric-queryd</a></h4>
    <p>
      metric-queryd is a simple HTTP service that executes SQL queries on a backend
      database and plots the results. The HTTP API allows you to create timeseries graphs
      and other visualizations using nothing but SQL. You can easily plug the returned
      charts into a HTML site to create a real-time SQL-powered dashboard.
    </p>
  </li>
</ul>

The two services, `metricd-collectd` and `metric-queryd` do not depend on each
other and don't share a lot of code. If you're only interested in the data capture
part or only in the visualization part, you can run only one of the services.

To get started with PlotFX, have a look at these pages:
[Getting Started](/documentation/getting-started),
[Data Model](/documentation/data-model)

<div class="notice">
  <div style="float:right;"><a class="github-button" data-style="mega" href="https://github.com/paulasmuth/fnordmetric" data-count-href="/paulasmuth/fnordmetric/stargazers" data-count-api="/repos/paulasmuth/fnordmetric#stargazers_count" data-count-aria-label="# stargazers on GitHub" aria-label="Star paulasmuth/fnordmetric on GitHub">View on GitHub</a></div>
  Join PlotFX development now on GitHub. We're always appreciating new pull requests, bugs and feature requests!
</div>


## Examples

Here are some example charts that were generated using `metric-queryd` and SQL to
give you some ideas on what is possible. Have a look at the
[full list of examples](/examples/) for more.

<div style="height: .6em;"></div>

_Example: Simple timeseries chart_
<a href="/examples/linecharts/simple_lines"><img src="/examples/linecharts/simple_lines/simple_lines.png" style="width: 100%;" /></a>

_Example: Chart with explicit domain and legend_
<a href="/examples/linecharts/explicit_domain"><img src="/examples/linecharts/explicit_domain/explicit_domain.png" style="width: 100%;" /></a>

_Example: Horizontal barchart_
<a href="/examples/barcharts/horizontal_bars"><img src="/examples/barcharts/horizontal_bars/horizontal_bars.png" style="width: 100%;" /></a>

_Example: Scatter chart with lables_
<a href="/examples/pointcharts/pointchart_with_labels"><img src="/examples/pointcharts/pointchart_with_labels/pointchart_with_labels.png" style="width: 100%;" /></a>

## Getting Started

To get started with PlotFX, have a look at the [Getting Started](/documentation/getting-started) and
[Data Model](/documentation/data-model) pages. If you have any questions please
don't hesitate to reach out via [Github Issues](http://github.com/paulasmuth/fnordmetric/issues)
or [the PlotFX google group](http://groups.google.com/group/fnordmetric).

<script async defer src="https://buttons.github.io/buttons.js"></script>
