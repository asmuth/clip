/api/v1/metrics/fetch_summary
=============================

Returns a summary timeseries computed by aggregating all (or some) series from
the metric.

###Parameters

<table>
  <tr>
    <td>metric_id</td>
    <td>The name of the metric for which to compute the summary.</td>
  </tr>
</table>

### Example Request

    >> POST /api/v1/metrics/fetch_summary HTTP/1.1
    >> Content-Type: application/json
    >> Content-Length: ...
    >>
    >> {
    >>   "metric_id": "users_online"
    >> }

### Example Response

    << HTTP/1.1 200 OK
    << Content-Length: ...
    <<
    << {
    << }

