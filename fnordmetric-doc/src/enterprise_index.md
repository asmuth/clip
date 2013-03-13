FnordMetric Enterprise
----------------------

FnordMetric Enterprise is a key-value store (much like redis or memcached) where each
key holds a "metric". There are different metric types like sum, mean, min/max, 90th
percentile, etcetra. You continuously add data to these keys/metrics which is aggregated
and periodically persisted.

FnordMetric Enterprise features a TCP/UDP, HTTP and WebSocket API, CSV/JSON Export and a
turnkey-ready HTML5 visualization solution ([FnordMetric UI](/documentation/ui_index)).
It can be used as a drop-in replacement for StatsD+Graphite.


### Semantics

There are three basic operations: `add\_sample`, `value\_at` and `values\_in`.
`add\_sample` adds a sample to a metric, `value\_at` retrieves the measured value at
a specified time and `values\_in` retrieves all aggregated measured values in a specified
time interval.

The metric type and interval are implicitly specified by the metric key; all keys have to
end with "$type-$interval".

For example: if you want a metric `response\_time` to record the average/mean of all sampled
values in an aggregation interval of 60 seconds, use the key `response\_time-mean-60`. For a
metric `total\_clicks` that sums up all measuements in one-hour intervals, you could use
`total_clicks.sum-3600`

You can find a list of all metric types and the [full API Reference here](/documentation/fnordmetric_api_reference/)


### Installation

Installing FnordMetric Enterprise is straightforward. Download the latest release
[here](/documentation/downloads) and run the jarfile with this command:

    java -jar FnordMetric-Enterprise-v1.2.7.jar --tcp 8922 --udp 8922 --websocket 8080

This will start FnordMetric, listen on UDP and TCP port 8922 and start a WebSocket
server in port 8080.


### Getting Started

...

