FnordMetric Enterprise
----------------------

FnordMetric Enterprise is a JVM-based timeseries database. It's a key-value store
(much like redis or memcached) where each key holds a "measurement". There are
different measurement methods like sum, mean, min/max, 90th percentile, etc. You
continuously add data to these keys/measurements which is aggregated and periodically
persisted.

FnordMetric Enterprise features disk persistence, a HTTP, TCP and UDP API, native
WebSockets support, CSV/JSON Export and a turnkey-ready HTML5 visualization solution
(FnordMetric UI). FnordMetric Enterprise can be used as a drop-in replacement for
StatsD+Graphite (it is a lot faster, see Benchmarks).


### Semantics

There are three basic operations: `add_sample`, `value_at` and `values_in` that
add a sample to an ongoing measurement, retrieve the measurement value at a
specified time, or retrieve all aggregated measurement values in a specified time
interval.

The measurement method and flush_interval are implicitly specified by the key;
all keys have to be postfixed with "$method-$flush_timeout". For example if
you want a key "response_time" to operate in average/mean mode and flush every 60
seconds, use the key `response_time-mean-60`, for a key "total_clicks" that
aggregates/sums a value and flushes every hour, you could use "total_clicks.sum-3600"



### In-memory vs. disk storage

FnordMetric Enterprise stores the values as 64bit double precision floats.

With an example flush timeout of 10 seconds, one metric uses 0.065 MB of
memory per day or 0.4 MB per week. The default ring buffer size is x,.

That means with only 4GB of ram, you could access the last month of data of
2500 counters/measurements with 10 second granularity all from the in-memory
ringbuffers (without ever hitting a HDD).

Requests that can not be served from memory require one sequential disk read.




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




TCP, UDP, WebSockets API
------------------------

protocol is line based, neither request nor response are allowed to
contain \n characters, but both request and response have to end with
a newline (\n) character. you can put multiple lines in one udp packet.
the protocol does not support multiplexing/pipelining.

example udp packet that increments four counters:

    SAMPLE my_counter1.sum-3600 123\n
    SAMPLE my_counter2.sum-3600 456\n
    SAMPLE my_counter3.sum-3600 789\n


### SAMPLE

samples/adds a value to a metric

format: SAMPLE [metric] [value]
response: "OK"

example:

    >> SAMPLE my_application.response_time.avg-30 23
    << OK



### VALUE_AT

retrieves the value of a metric at one point in time

format: VALUE_AT [metric] [at]
response: Float

example:

     >> VALUE_AT my_application.response_time.avg-30 -20min
     << 17.42



### VALUES_IN

retrieves all values of a metric in a time interval

format: VALUE_AT [metric] [from] [until]
response: Comma-seperated Timestamp:Float tuples

example:

     >> VALUE_AT my_application.response_time.avg-30 -5min now
     << 1360804571:4233.52,1360804581:4312.36,1360804591:6323.12,




License
-------

FnordMetric Enterprise is MIT licensed like all other FnordMetric products.





+ Usage
+ HTTP API
+ TCP/UDP API




