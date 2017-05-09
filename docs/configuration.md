Configuration File
==================

This is a list of all supported top-level stanzas in the configuration file.
For more information on the semantics of the statements, please refer to the
linked pages.

<table>
  <thead>
    <tr>
      <th>Setting</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>backend</strong></code></td>
      <td>Connection string/URL for the backend database (e.g. `mysql://localhost:3306/mydb?user=root`)</td>
    </tr>
    <tr>
      <td><code><strong>include</strong></code></td>
      <td></td>
    </tr>
    <tr>
      <td><code><strong>metric</strong></code></td>
      <td>Declares a metric. See <a href="/documentation/metric-configuration">Metric Configuration</a></td>
    </tr>
    <tr>
      <td><code><strong>unit</strong></code></td>
      <td>Declares a unit of measurement. See <a href="/documentation/units">Units of Measurement</a></td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-statsd">listen_udp</a></strong></code></td>
      <td>Declares a UDP/statsd listener port (A data source).</td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-http">listen_http</a></strong></code></td>
      <td>Declares a HTTP listener port (A data source).</td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-http">fetch_http</a></strong></code></td>
      <td>Pull data via HTTP (A data source).</td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-shell">fetch_proc</a></strong></code></td>
      <td>Pull data via HTTP (A data source).</td>
    </tr>
    <tr>
      <td><code><strong>dashboard</strong></code></td>
      <td>Declares a dashboard. See <a href="/documentation/dashboards">Dashboard</a></td>
    </tr>
    <tr>
      <td><code><strong>labels</strong></code></td>
      <td>Add a global label (this label will apply to all metrics)</td>
    </tr>
    <tr>
      <td><code><strong>label_set</strong></code></td>
      <td>Set a (default) label value.  See <a href="/documentation/rewrite-rules">Rewrite Rules</a></td>
    </tr>
  </tbody>
</table>


### Configuration File Locations

By default, the `metrictl` program will try to find the configuration file in
these locations in the order in which they are listed:

    ~/.metricttl
    /etc/metrictools/metrictl.conf
    /etc/metrictl.conf

The `metricd` program will try these locations in order:

      /etc/metrictools/metricd.conf
      /etc/metricd.conf

You can always explicitly specify the config file location using the `--config`
switch, for example:

    $ metrictl --config path/to/metrics.conf ...
    $ metricd --config path/to/metrics.conf ...


### Full Example


    backend "sqlite:///tmp/test.sqlite"

    labels "hostname"
    label_set hostname localhost default

    include "./code/metrictools/etc/units-default.conf"
    include "./code/metrictools/plugins/linux/linux-plugin.conf"

    collect_http {
      url "http://localhost:9175/eventql/stats"
      interval 10s
      format statsd
      label_set hostname eventql1
    }
