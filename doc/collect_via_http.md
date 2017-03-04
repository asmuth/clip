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
or `json` format. for more information on the format see the [sample formats](/documentation/sample-format)
page.


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
      <td><code><strong>bind</strong></code></td>
      <td>The address on which the HTTP server should be started (default: 0.0.0.0)</td>
    </tr>
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

To insert a sample, send a `POST /metrics` request. By default, the API expects
the samples to be in Text/StatsD in the body of the request. You can switch
to the JSON format by sending a `Content-Type: application/json` header.

    >> POST /metrics
    >> Content-Type: text/plain
    >>
    >> metric_name1:value2\n
    >> metric_name2:value2\n

    << HTTP/1.1 201 CREATED

A simple way to send in metrics via HTTP from your command line if you have
metric-collectd running on HTTP port 8080 would be using the curl utility:

    $ curl -X POST -d "host_stats.request_count:23.5" localhost:8080/metrics

This would insert the value "23.5" into the metric "host_stats.request_count".

A sample can optionally be labelled with one or more "labels". To attach labels,
put them all into the curly braces separated by commas. For example, if we want
to sample the CPU utilization on multiple hosts into the metric "cpu-utilization"
with two label dimensions "hostname" and "datacenter", our UDP packets could look
like this:


    >> POST /metrics
    >> Content-Type: text/plain
    >>
    >> cpu-utilization{hostname=machine83,datacenter=ams1}:0.642\n

    << HTTP/1.1 201 CREATED


To send the above request with curl:

    $ curl -X POST -d "cpu-utilization{hostname=machine83,datacenter=ams1}:0.642" localhost:8080/metrics

For more information on the format see the [Sample Formats](/documentation/sample-format)
page.


