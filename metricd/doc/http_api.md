FnordMetric Server HTTP API
===========================

GET /query
----------

Execute a ChartSQL / SQL query and return the result.

#### Parameters:

<table>
  <tr>
    <th>q <i>(mandatory)</i></th>
    <td>
      the ChartSQL / SQL query to execute
    </td>
  </tr>
  <tr>
    <th>format</th>
    <td>
      the response format: `svg` or `json` (default: json)
    </td>
  </tr>
  <tr>
    <th>width</th>
    <td>
      the width of the chart to be rendered in px (default: 800)
    </td>
  </tr>
  <tr>
    <th>height</th>
    <td>
      the height of the chart to be rendered in px (default: 320)
    </td>
  </tr>
</table>
<br />


POST /query
-----------

Execute a ChartSQL / SQL query and return the result. Send the ChartSQL query as
the HTTP POST body and use the same GET query parameters as for the
`GET /query` call


GET /metrics
------------

Returns a list of all metrics.

Parameters:

<table>
  <tr>
    <th>format</th>
    <td>
      the response format: `csv`, or `json` (default)
    </td>
  </tr>
</table>
<br />

Examples:

    >> GET /metrics HTTP/1.1
    << HTTP/1.1 200 OK
    << ...
    << {
    <<   "metrics": [
    <<     {
    <<       "key": "http_status_codes",
    <<       "labels": ["statuscode", "hostname"],
    <<       "last_insert": 1414241420602240,
    <<       "total_stored_bytes": 24641536
    <<     },
    <<     {
    <<       "key": "http_latency",
    <<       "labels": ["url", "hostname"],
    <<       "last_insert": 1414241420602240,
    <<       "total_stored_bytes": 129499136
    <<     },
    <<   ]
    << }


### POST /metrics

Insert a sample into a metric. If no metric with this key exists, a new one
will be created.

#### Parameters:

<table>
  <tr>
    <th>metric <i>(mandatory)</i></th>
    <td>
      the key of this metric (e.g. my-counter-sum-15 or /frontend/http_status_code)
    </td>
  </tr>
  <tr>
    <th>value <i>(mandatory)</i></th>
    <td>
      the value to add/sample to this metric
    </td>
  </tr>
</table>
<br />

Examples:

    >> POST /metrics/total_sales_in_euro-sum-30?value=351 HTTP/1.1
    << HTTP/1.1 201 CREATED

    >> POST /metrics/http_status_codes?value=351&label[statuscode]=200&label[hostname]=myhost1 HTTP/1.1
    << HTTP/1.1 201 CREATED




