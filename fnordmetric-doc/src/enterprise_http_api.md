HTTP API
--------

### GET /metric/:key

parameters:

    key:     key of this metric (e.g. my-counter-sum-15)
    format:  response format csv, json or xml (also use HTTP-Accept)
    at:      if set, returns a single value at this point in time. content can be a timestamp
             or a timespec like -3m... mutually exclusive with since/until
    since/until: when the since and until parameters are set, all values in the supplied time
             interval are returned. values can be a timestamp or a timespec like... since/until
             are mutually exclusive with at

examples:

    >> GET /metric/total_sales_in_euro-sum-30?at=-5m&format=json
    << HTTP/1.1 200 OK
    << ...
    << { "value": 23426 }

    >> GET /metric/total_sales_in_euro-sum-30?since=-2min&until=now&format=json
    << HTTP/1.1 200 OK
    << ...
    << { "values": { 1360804571: 4233, 1360804581: 4636, 1360804591: 3621, ... } }



### POST /metric/:key

   key:      key of this metric (e.g. my-counter-sum-15)
   value:    the value to add/sample to this metric

examples:

    >> POST /metric/total_sales_in_euro-sum-30?value=351
    << HTTP/1.1 201 CREATED




