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


### FnordMetric Enterprise vs. StatsD

+ allows for flush intervals as low as one second
+ rendered in the browser, interactive
+ much much more scalable
+ highly customizable with css
+ requires only a single deployment
+ i18n (proper timezones in graphs due to in browser rendering etc)
