FnordMetric Enterprise
----------------------

FnordMetric Enterprise is a key-value store (much like redis or memcached) where each
key holds a "metric". There are different metric types like sum, mean, min/max, 90th
percentile, etcetra. You continuously add data to these keys/metrics which is aggregated
and periodically persisted.

### Installation

Installing FnordMetric Enterprise is straightforward. [Download the latest release
here](/documentation/downloads) and run the jarfile with this command:

    java -jar FnordMetric-Enterprise-v1.2.7.jar --tcp 8922 --websocket 8080 --admin 8081

This will start FnordMetric, listen on TCP port 8922, start a WebSocket server on port
8080 and start the admin interface on port 8081.


### Semantics

Each metric is addressed by a unique key. The metrics type and interval are implicitly specified
by its key; all keys have to end with a "$type-$interval" pattern.

For example: if you want a metric `response\_time` to record the average/mean of all sampled
values in an aggregation interval of 60 seconds, you'd use the key `response\_time-mean-60` (type: mean,
interval: 60s). For a metric `total\_clicks` that sums up all measuements in one-hour intervals, you would use
`total_clicks.sum-3600` (type: sum, interval: 3600s)

You can find a list of all metric types and the [full API Reference here](/documentation/fnordmetric_api_reference/)


### Getting Started

As an example, we will report the average http response time of a fictional application "myapp" to
FnordMetric. They key we will use for this is `myapp.response\_time-mean-10` which will create a
metric "myapp.response_time" that records the mean of all sampled values in a 10 second interval.

To add the first value to this metric we use netcat from the command line. This opens up a TCP
connection to localhost on port 8922 and sends one line to add the value 42 to the metric. In our
example this means we record the response time of one HTTP request that took 42ms.

FnordMetric Enterprise will automatically create the metric if it doesnt exist yet.

    $ echo "SAMPLE myapp.response_time-mean-10" | nc localhost 9222
    OK

There are also [other ways to connect to FnordMetric Enterprise](/documentation/fnordmetric_api_reference/)
(e.g. WebSockets, UDP, HTTP).

You should now be able to navigate to the admin interface on http://localhost:8081/ in your
browser and see something like this:

<img src="/img/simple_example_screen.png" width="630" class="shadow" />
<br />

We can retrieve the measured data in a similar way. This will e.g. retrieve the measured mean
response times for the last hour ([see a full list of commands here](/documentation/fnordmetric_api_reference/))

    $ echo "VALUESIN myapp.response_time-mean-10 -1h now" | nc localhost 9222
    1360804571:4233.52 1360804581:4312.36 1360804591:6323.12


You now have a running FnordMetric application. There is a lot more you can do: Checkout out
the [full API Reference here](/documentation/fnordmetric_api_reference/) or [read more
about FnordMetric UI](/documentation/ui_index), a JavaScript / HTML5 library that you can use
to plug in the data into any webpage within seconds.
