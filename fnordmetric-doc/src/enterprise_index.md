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

The measurement method is implicitly specified by the key; all keys need to end
with one of the method postfixes. For example `my_key.sum` or `my_key.avg` for
sum and average measurements.


### Usage Example

Example: sample three values and then receive the current average value:

    >> SAMPLE my_application.response_time.avg 23
    << OK
    >> SAMPLE my_application.response_time.avg 42
    << OK
    >> SAMPLE my_application.response_time.avg 17
    << OK
    >> VALUE_AT my_application.response_time.avg NOW
    << 27.3



### In-memory vs. disk storage

FnordMetric Enterprise stores the values as 64bit double precision floats.

The default flush timeout is 10 seconds, that means per default every key
uses 0.065 MB of memory per day or 0.4 MB per week. The default setting is
to keep the last 4 weeks of data in-memory.

That means with only 4GB of ram, you could access the last month of data of
2500 counters/measurements with 10 second granularity without ever hitting a
HDD.

All requests that can not be served from memory are read from disk with one
sequential read.




License
-------

FnordMetric Enterprise is MIT licensed like all other FnordMetric products.





+ Usage
+ HTTP API
+ TCP/UDP API




