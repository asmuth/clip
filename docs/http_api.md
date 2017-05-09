HTTP API
========

The HTTP API currently does not support any kind of authentication. These are
the API methods exposed by `metricd`

<table>
  <tr>
    <th>Path</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><code><a href="/documentation/metrictl-list"><strong>POST /api/v1/list</strong></a></code></td>
    <td>The list method allows you to list all metrics and their instances and latest values.</td>
  </tr>
  <tr>
    <td><code><a href="/documentation/metrictl-log"><strong>POST /api/v1/log</strong></a></code></td>
    <td>The log method allows you to retrieve a logfile of collected measurements for a specific metric.</td>
  </tr>
  <tr>
    <td><code><a href="/documentation/metrictl-plot"><strong>POST /api/v1/plot</strong></a></code></td>
    <td>The plot method allows you to draw a chart from one or more metrics.</td>
  </tr>
  <tr>
    <td><code><a href="/documentation/metrictl-insert"><strong>POST /api/v1/insert</strong></a></code></td>
    <td>The insert method allows you to add a measurement to a metric.</td>
  </tr>
</table>
