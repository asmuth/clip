Collecting Data via HTTP
========================

You can either send data to metric-collectd using the HTTP API, or configure
metric-collectd to pull from your HTTP endpoint.


HTTP Pull
---------

To fetch ("scrape") samples via HTTP, add a `fetch_http` block to your configuration
file. `metric-collectd` will peridodically connect to the specified url and
collect metrics.

    fetch_http {
      url "http://localhost:9175/stats"
      interval 10s
      format statsd
    }

This is the list of valid stanzas within the `listen_http` block:

<table>
  <thead>
    <tr>
      <th>Setting</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>url</strong></code></td>
      <td>The HTTP URL to fetch</td>
    </tr>
    <tr>
      <td><code><strong>format</strong></code></td>
      <td>The expected input sample format ('statsd' or 'json')</td>
    </tr>
    <tr>
      <td><code><strong>interval</strong></code></td>
      <td>Controls how often the URL is fetched. Valid values are any number (interpreted as seconds) or strings of the format
`N[s(((ec)ond)s)|m(((in)ute)s)|h((our)s)|d((ay)s)|w((eek)s)|y((ear)s)]`. For
example: `1s`, `30min` or `6hours`.</td>
    </tr>
    <tr>
      <td><code><strong>rewrite</strong></code></td>
      <td>Rewrite the metric name or labels (see <a href="/documentation/configuration-file#rewrite">Configuration File</a>)</td>
    </tr>
  </tbody>
</table>

metric-collectd will sends a `HTTP GET` request for the specified URL and expects
a 200-range response status code. The response body must be either in `statsd`
or `json` format.


HTTP Push
---------

To listen for samples via HTTP, add a `listen_http` block to your configuration
file.`metric-collectd` will listen for HTTP connections on the specified port.

    listen_http {
      port 8080
    }

This is the list of valid stanzas within the `listen_http` block:

<table>
  <thead>
    <tr>
      <th>Setting</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>port</strong></code></td>
      <td>The port on which the HTTP server should be started</td>
    </tr>
    <tr>
      <td><code><strong>rewrite</strong></code></td>
      <td>Rewrite the metric name or labels (see <a href="/documentation/configuration-file#rewrite">Configuration File</a>)</td>
    </tr>
  </tbody>
</table>

To insert a sample, send a `POST /metrics` request. There are two mandatory
parameters `metric` and `value`: The metric parameter must contain the metric
name to insert into and the value parameter must contain the value to insert.

    POST /metrics
    metric=<metric-name>&value=<value>

A simple way to send in metrics via HTTP from your command line if you have
metric-collectd running on HTTP port 8080 would be using the curl utility:

    $ curl -X POST -d "metric=host_stats.request_count&value=23.5" localhost:8080/metrics

This would insert the value "23.5" into the metric "host_stats.request_count".

A sample can optionally be labelled with one or more "labels". Each label is a
key: value pair and is specified as an additional HTTP POST parameter where the
key follows this format: `label[...]`

    POST /metrics
    metric=<metric-name>&value=<value>&label[<k1>]=<v1>

    POST /metrics
    metric=<metric-name>&value=<value>&label[<k1>]=<v1>&label[<k2>]=<v2>

For example, if we want to sample the CPU utilization on multiple hosts into
the metric "cpu-utilization" with two label dimensions "hostname" and "datacenter",
our HTTP requests should look like this:

    >> POST /metrics?metric=cpu-utilization&value=0.642&label[hostname]=machine83&label[datacenter]=ams1
    << HTTP/1.1 201 CREATED

Alas, curl interprets square brackets as globs, so we have to escape the square
brackets when using the curl command line utility:

    $ curl -X POST -d "metric=cpu-util&label\[hostname\]=machine83&label\[datacenter\]=ams1&value=0.642" localhost:8080/metrics

More examples:

    >> POST /metrics?metric=total_sales_in_euro-sum-30&value=351 HTTP/1.1
    << HTTP/1.1 201 CREATED

    >> POST /metrics?metric=http_status_codes&value=351&label[statuscode]=200&label[hostname]=myhost1 HTTP/1.1
    << HTTP/1.1 201 CREATED

