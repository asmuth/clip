HTTP API
--------

FIXPAUL: blah blah

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
      when the since and until parameters are set, all values in the supplied time
      interval are returned. values can be a timestamp or a timespec like... since/until
      are mutually exclusive with at
    </td>
  </tr>
  <tr>
    <th>at</th>
    <td>
      if set, returns a single value at this point in time. content can be a timestamp
      or a timespec like -3m... mutually exclusive with since/until
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
    << { "value": 23426 }

    >> GET /metric/total_sales_in_euro-sum-30?since=-2min&until=now&format=json
    << HTTP/1.1 200 OK
    << ...
    << { "values": { 1360804571: 4233, 1360804581: 4636, 1360804591: 3621, ... } }


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




