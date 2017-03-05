HTTP API
========

The HTTP API currently does not support any kind of authentication

## metric-collectd

These are the API methods exposed by `metric-collectd`

<table>
  <tr>
    <th>Path</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><code><a href="/documentation/collect-data-via-http"><strong>POST /metrics</strong></a></code></td>
    <td>Submit input samples</td>
  </tr>
</table>

## metric-queryd

These are the API methods exposed by `metric-queryd`

<table>
  <tr>
    <th>Path</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><code><a><strong>GET /api/v1/query</strong></a></code></td>
    <td>Execute a SQL query</td>
  </tr>
  <tr>
    <td><code><a href="/documentation/widget-timeseries"><strong>GET /embed/timeseries</strong></a></code></td>
    <td>Render a timeseries widget</td>
  </tr>
</table>
