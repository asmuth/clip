HTTP API
--------

### GET /metrics

Returns a list of all metrics.

#### Parameters:

<table>
  <tr>
    <th>format</th>
    <td>
      the response format: `csv`, or `json` (default)
    </td>
  </tr>
</table>
<br />

#### Examples:

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


### GET /query

Execute a ChartSQL / SQL query and return the result.

#### Parameters:

<table>
  <tr>
    <th>query <i>(mandatory)</i></th>
    <td>
      the ChartSQL / SQL query to execute
    </td>
  </tr>
  <tr>
    <th>format</th>
    <td>
      the response format: `svg`, `csv`, or `json` (default)
    </td>
  </tr>
</table>
<br />

#### Examples:

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


### GET /metric/:key

Retrieves a metric's value(s) at a specified point in time or in a specified time interval

Parameters:

<table>
  <tr>
    <th>key <i>(mandatory)</i></th>
    <td>
      key of this metric (e.g. my-counter-sum-15)
    </td>
  </tr>
  <tr>
    <th>since / until</th>
    <td>
      When the since and until parameters are set, all values in the supplied time
      interval are returned.  since/until are mutually exclusive with at.
      Possible values are strings like "now", "-30min" or "-6h". You can find the <a href="/documentation/enterprise_api_reference"/>full documentation for this time format here</a>
    </td>
  </tr>
  <tr>
    <th>at</th>
    <td>
      if set, returns a single value at this point in time. at is mutually exclusive with since/until
      Possible values are strings like "now", "-30min" or "-6h". You can find the <a href="/documentation/enterprise_api_reference"/>full documentation for this time format here</a>
    </td>
  </tr>
  <tr>
    <th>format</th>
    <td>
      response format csv, json or xml (also use HTTP-Accept)
    </td>
  </tr>
</table>
<br />

Examples:

    >> GET /metric/total_sales_in_euro-sum-30?at=-5m&format=json
    << HTTP/1.1 200 OK
    << ...
    << { "value": 23426 }

    >> GET /metric/total_sales_in_euro-sum-30?since=-2min&until=now&format=json
    << HTTP/1.1 200 OK
    << ...
    << { "values": { 1360804571: 4233, 1360804581: 4636, 1360804591: 3621, ... } }


### POST /metric/:key

Sample/add a value to a metric.

Parameters:

<table>
  <tr>
    <th>key <i>(mandatory)</i></th>
    <td>
      key of this metric (e.g. my-counter-sum-15)
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

    >> POST /metric/total_sales_in_euro-sum-30?value=351
    << HTTP/1.1 201 CREATED



