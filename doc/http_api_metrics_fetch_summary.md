/api/v1/metrics/fetch_summary
=============================

Returns a summary timeseries computed by aggregating all (or some) series from
the metric.

###Parameters

<table>
  <tr>
    <td><code><strong>metric_id</strong></code></td>
    <td>The name of the metric for which to compute the summary.</td>
  </tr>
  <tr>
    <td><code><strong>from</strong></code></td>
    <td>Begin of the query time window (inclusive). Valid values are: ISO-8601-style dates, unix timestamps, `now` and relative references of the form `-NN[s(((ec)ond)s)|m(((in)ute)s)|h((our)s)|d((ay)s)|w((eek)s)|y((ear)s)]`. For example: `2017-01-29 14:00:00`, `now`, `-30min` or `-6h`</td>
  </tr>
  <tr>
    <td><code><strong>until</strong></code></td>
    <td>End of the time window (exclusive). See "from" (above) for list of valid values.</td>
  </tr>
  <tr>
    <td><code><strong>summarize_gross</strong></code></td>
    <td>
      Return one or more <a href="/documentation/metrics/summarization">gross summaries</a>
      for the results of this query. Value may be a single string, an array
      of strings or string containing a comma-separated list of values. For valid summarization methods please see the
      <a href="/documentation/metrics/summarization">Summarization Page</a>
    </td>
  </tr>
  <tr>
    <td><code><strong>summarize_group</strong></code></td>
    <td>
      Set the  <a href="/documentation/metrics/summarization">group summary method</a>
      for this query. Value must be a string. For valid summarization methods please see the
      <a href="/documentation/metrics/summarization">Summarization Page</a>
    </td>
  </tr>
</table>

### Example Request

    >> POST /api/v1/metrics/fetch_summary HTTP/1.1
    >> Content-Type: application/json
    >> Content-Length: ...
    >>
    >> {
    >>   "metric_id": "users_online",
    >>   "from": "-2hours",
    >>   "until": "now"
    >> }

### Example Response

    << HTTP/1.1 200 OK
    << Content-Length: ...
    <<
    << {
    << }

