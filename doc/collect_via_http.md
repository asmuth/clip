Collecting Data via HTTP
========================

You can either send data to using the HTTP API, or configure metricd to pull
from your HTTP endpoint.


HTTP Pull
---------

To fetch ("scrape") samples via HTTP, add a `collect_http` block to your configuration
file. `metricd` will peridodically connect to the specified url and collect metrics.

    collect_http {
      url "http://localhost:9175/stats"
      interval 10s
      format statsd
    }

This is the list of valid stanzas within the `collect_http` block:

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
      <td>The expected input sample format ('statsd', 'borgmon' or 'json')</td>
    </tr>
    <tr>
      <td><code><strong>interval</strong></code></td>
      <td>Controls how often the URL is fetched. Valid values are any number (interpreted as seconds) or strings of the format
`N[s(((ec)ond)s)|m(((in)ute)s)|h((our)s)|d((ay)s)|w((eek)s)|y((ear)s)]`. For
example: `1s`, `30min` or `6hours`.</td>
    </tr>
    <tr>
      <td><code><strong>label_set</strong></code></td>
      <td>Set a (default) label value.  See <a href="/documentation/rewrite-rules">Rewrite Rules</a></td>
    </tr>
  </tbody>
</table>

metricd sends a `HTTP GET` request for the specified URL and expects a 200-range
response status code. The response body must be either in `statsd`
or `json` format. for more information on the format see the [sample formats](/documentation/sample-format)
page.


HTTP Push
---------

To listen for samples via HTTP, add a `listen_http` block to your configuration
file.`metricd` will listen for HTTP connections on the specified port.

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
      <td><code><strong>label_set</strong></code></td>
      <td>Set a (default) label value.  See <a href="/documentation/rewrite-rules">Rewrite Rules</a></td>
    </tr>
  </tbody>
</table>

To insert a sample, send a `POST /api/v1/insert` request. By default, the API expects
the samples to be in Text/StatsD in the body of the request. You can switch
to the JSON format by sending a `Content-Type: application/json` header.

    >> POST /api/v1/insert
    >> Content-Type: text/plain
    >>
    >> metric_name1:value2\n
    >> metric_name2:value2\n

    << HTTP/1.1 201 CREATED

A simple way to send in metrics via HTTP from your command line if you have
metricd running on HTTP port 8080 would be using the curl utility:

    $ curl -X POST -d "host_stats.request_count:23.5" localhost:8080/api/v1/insert

This would insert the value "23.5" into the metric "host_stats.request_count".

A sample can optionally be labelled with one or more "labels". To attach labels,
put them all into the curly braces separated by commas. For example, if we want
to sample the CPU utilization on multiple hosts into the metric "cpu-utilization"
with two label dimensions "hostname" and "datacenter", our UDP packets could look
like this:


    >> POST /api/v1/insert
    >> Content-Type: text/plain
    >>
    >> cpu-utilization{hostname=machine83,datacenter=ams1}:0.642\n

    << HTTP/1.1 201 CREATED


To send the above request with curl:

    $ curl -X POST -d "cpu-utilization{hostname=machine83,datacenter=ams1}:0.642" localhost:8080/api/v1/insert

For more information on the format see the [Input Formats](/documentation/sample-format)
page.


