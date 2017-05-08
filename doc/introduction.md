# metrictools v0.8.0

metrictools is a collection of lightweight tools that allow you to record measurements
and build beautiful real-time dashboards within minutes. **If you're new to metrictools it's highly recommended that you start with the
[Getting Started](/documentation/getting-started) page.**. 

Here's a quick rundown of the key features for people who don't like reading
documentation.:

  - Add your own custom metrics and labels
  - Insert data, run queries and create charts using a standalone command line interface
  - Pull ("scrape") measurements via HTTP, shell scripts or SNMP or push via HTTP or UDP/statsd
  - Include charts and tables in any HTML page using the HTTP API
  - All metrics are explicitly configured and have a number of rich settings (like units of measurement)
  - Data is stored in a pluggable storage backend (SQLite, MySQL, Postgres, EventQL)
  - Optional background daemon that runs data collection and exposes a HTTP API
  - Plugins can contain metric configuration, exporters and dashboards (check out
    the linux plugin)
  - Implemented as self-contained C++; quick to set up and does not require a lot of resources
  - The metrictools project was started over 7 years ago and is still actively maintained


<div class="notice">
  <div style="float:right;"><a class="github-button" data-style="mega" href="https://github.com/paulasmuth/metrictools" data-count-href="/paulasmuth/metrictools/stargazers" data-count-api="/repos/paulasmuth/metrictools#stargazers_count" data-count-aria-label="# stargazers on GitHub" aria-label="Star paulasmuth/metrictools on GitHub">View on GitHub</a></div>
  Join metrictools development now on GitHub. We're always appreciating new pull requests, bugs and feature requests!
</div>


### Commands

This is a list of the `metrictl` commands and a quick description for each of
them. For more detailed information you're kindly referred to the following
pages.

<table>
  <thead>
    <tr>
      <th>Command</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>metrictl list</strong></code></td>
      <td>Lorem ipsum dolor sit amet</td>
    </tr>
    <tr>
      <td><code><strong>metrictl log</strong></code></td>
      <td>Lorem ipsum dolor sit amet</td>
    </tr>
    <tr>
      <td><code><strong>metrictl plot</strong></code></td>
      <td>Lorem ipsum dolor sit amet</td>
    </tr>
    <tr>
      <td><code><strong>metrictl table</strong></code></td>
      <td>Lorem ipsum dolor sit amet</td>
    </tr>
    <tr>
      <td><code><strong>metrictl histogram</strong></code></td>
      <td>Lorem ipsum dolor sit amet</td>
    </tr>
    <tr>
      <td><code><strong>metrictl insert</strong></code></td>
      <td>Lorem ipsum dolor sit amet</td>
    </tr>
    <tr>
      <td><code><strong>metrictl collect</strong></code></td>
      <td>Lorem ipsum dolor sit amet</td>
    </tr>
  </tbody>
</table>

### Datastores

The currently supported data stores are 
[SQLite](/documentation/backend-sqlite),
[MySQL](/documentation/backend-mysql), 
[Postgres](/documentation/backend-postgres) and [EventQL](/documentation/backend-eventql).
If you'd like to add a new backend, please have a look at `src/metrictools/storage` and
create a pull request on GitHub.


### Support

To get started with metrictools, have a look at the [Getting Started](/documentation/getting-started),
[Data Model](/documentation/data-model), [Metric Types](/documentation/metric-types)
and [Federation](/documentation/federation) pages. If you have any questions please
don't hesitate to reach out via [Github Issues](http://github.com/paulasmuth/metrictools/issues)
or [the metrictools google group](http://groups.google.com/group/metrictools).

<script async defer src="https://buttons.github.io/buttons.js"></script>
