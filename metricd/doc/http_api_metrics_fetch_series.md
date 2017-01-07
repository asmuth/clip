/api/v1/metrics/fetch_series
=============================

Returns timeseries data from one or more series of the metrics.

###Parameters

<table>
  <tr>
    <td>metric_id</td>
    <td>The name of the metric from which to read the series.</td>
  </tr>
</table>

### Example Request

    >> POST /api/v1/metrics/fetch_series HTTP/1.1
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

