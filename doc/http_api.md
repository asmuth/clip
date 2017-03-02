HTTP API
========

The HTTP API gives you full access to metricd via HTTP. All metricd operations
can be executed via this API. The metricctl cli also uses the HTTP API to access
metricd.

#### Version Prefix

All API routes are prefixed with '/api/v1/'. This documentation is only for the
v1 version of the API.

**Backwards compatibility:<** At the current version, EventQL does not yet
promise backwards compatibility even with the v1 prefix. We'll remove this warning
when this policy changes. We expect we'll reach API stability by metricd 1.0.

<h3>API Methods</h3>
<table class="api_reference http">
  <tr>
    <td><code><a href="../metrics_list"><strong>/api/v1/metrics/list</strong></a></code></td>
    <td></td>
  </tr>
  <tr>
    <td><code><a href="../metrics_list_series"><strong>/api/v1/metrics/list_series</strong></a></code></td>
    <td></td>
  </tr>
  <tr>
    <td><code><a href="../metrics_fetch_series"><strong>/api/v1/metrics/fetch_series</strong></a></code></td>
    <td></td>
  </tr>
  <tr>
    <td><code><a href="../metrics_fetch_summary"><strong>/api/v1/metrics/fetch_summary</strong></a></code></td>
    <td>Fetch a summary timeseries of all series in a metric.</td>
  </tr>
  <tr>
    <td><code><a href="../metrics_insert"><strong>/api/v1/metrics/insert</strong></a></code></td>
    <td></td>
  </tr>
</table>
