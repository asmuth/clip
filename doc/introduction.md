# FnordMetric v0.8.0

FnordMetric (metricd) is collection of lightweight tools for real-time metrics
collection and visualization with SQL. It allows you to capture measurements and
build beautiful real-time dashboards within minutes. All using your favorite SQL
database.

<ul>
  <li>
    <h4>metric-collectd</h4>
    <p>
      metric-collectd is a lightweight daemon that collects measurements and other
      timeseries data, pre-aggregates it and stores the resulting tuples into a
      backend database.
    </p>
    <p>
      metric-collectd can currently listen for samples in text/statsd or JSON format
      via UDP or HTTP. It can also pull samples via HTTP or SNMP.
    </p>
  </li>

  <li>
    <h4>metric-queryd</h4>
    <p>
      metric-queryd is a simple HTTP service that executes SQL queries on a backend
      database and plots the results. The HTTP API allows you to create timeseries graphs
      and other visualizations using nothing but SQL.
    </p>
    <p>
      You can easily plug the returned charts into a HTML site to create a real-time
      SQL-powered dashboard.
    </p>
  </li>
</ul>

The two services, `metricd-collectd` and `metric-queryd` do not depend on each
other and don't share a lot of code. If you're only interested in the data capture
part or only in the visualization part, you can run only one of the services.

To get started with FnordMetric, have a look at these pages: [Getting Started]()

<div class="notice">
  <div style="float:right;"><a class="github-button" data-style="mega" href="https://github.com/paulasmuth/fnordmetric" data-count-href="/paulasmuth/fnordmetric/stargazers" data-count-api="/repos/paulasmuth/fnordmetric#stargazers_count" data-count-aria-label="# stargazers on GitHub" aria-label="Star paulasmuth/fnordmetric on GitHub">View on GitHub</a></div>
  Join FnordMetric development now on GitHub. We're always appreciating new pull requests, bugs and feature requests!
</div>


## Web Interface

Optionally, `metric-queryd` can expose a web interface that allows you to execute
SQL queries and create charts:

<div style="width:820px;margin:1em auto 3em auto;height: 495px;overflow: hidden;border: 1px solid #ccc;border-radius: 3px;box-shadow: 1px 2px 6px rgba(0,0,0,.1);" class="shadow">
  <video width="820" autoplay="autoplay" loop>
    <source src="/fnordmetric-server.mp4" type="video/mp4">
    <script type="text/javascript">
      if (!(navigator.userAgent.search("Safari") >= 0 && navigator.userAgent.search("Chrome") < 0)) {
        document.write('<source src="/fnordmetric-server.webm" type="video/webm">');
      }
    </script>
    <img src="/img/fnordmetric_server_screen.png" width="875" />
  </video>
</div>


## Supported Databases

These are the currently supported database. If you'd like to add a new backend,
please have a look at `src/fnordmetric/backends` and create a pull request on
GitHub.

  - [MySQL]()
  - [Postgres]()
  - [EventQL]()


## Getting Started

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor
incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis
nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu
fugiat nulla pariatur

<script async defer src="https://buttons.github.io/buttons.js"></script>
