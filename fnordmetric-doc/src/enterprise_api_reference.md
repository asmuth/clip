API Reference
-------------

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

These are all metric types that are currently supported:

<table>
  <tr>
    <th>sum</th>
    <td>
      Records the sum of all sampled values in an interval. <i>e.g. total_sales-sum-60</i>
    </td>
  </tr>
  <tr>
    <th>mean</th>
    <td>
      Records the mean / average of all sampled values in an interval. <i>e.g response_time-mean-60</i>
    </td>
  </tr>
</table>


### Protocol

FnordMetric Enteprise offers a simple US-ASCII text-based interface. It is a very simple
line-based serial request/response protocol. Requests may contain all ASCII characters
and must end with a newline (`\n`). Responses are also simple ASCII formatted numbers,
seperated with whitespaces and colons. Responses also always end with a newline character.

_Example: Increment the total\_requests-sum-60 by 5_

    >> SAMPLE total_requests-sum-60 5
    << OK

_Example: Retrieve the current value of total\_requests-sum-60_

    >> VALUEAT total_requests-sum-60 now
    << 23087


#### TCP / Websockets

When connecting with TCP, there is no application level handshake: you just open up the
connection and start sending commands. The protocol does not support multiplexing / pipelining:
You have to wait for one response line after every request you send.

Example with netcat:

    echo "SAMPLE total_requests-sum-60" | nc localhost 8922


When connecting with Websockets, you have to use `fnordmetric_enterprise` as the protocol string.

Example in JavaScript:

    var ws = new WebSocket("ws://localhost:8080/", "fnordmetric_enterprise")
    ...
    ws.send("SAMPLE total_requests-sum-60")


The generic error response format for both TCP and WebSocket connections is (the error message
will never contain a newline character).

    ERROR something went wrong\n


#### UDP:

When you are using UDP as the transport layer you won't receive any responses. Therefore the only
command that works over UDP is "sample". Note that UDP doesn't guarantee delivery.

You can put one or more commands into one UDP packet. Every command needs to end with a newline
character (`\n`).

Example UDP packet that increments three counters:

    SAMPLE my_counter1.sum-3600 123\n
    SAMPLE my_counter2.sum-3600 456\n
    SAMPLE my_counter3.sum-3600 789\n



### Time format

  here be dragons



### Commands

#### SAMPLE

Samples/adds a value to a metric. What exactly this does depends on the type of the metric.

*Format:*

    SAMPLE [metric] [value]

*Response:*

    "OK"

*Example:*

    >> SAMPLE my_application.response_time.avg-30 23
    << OK

<br />


#### VALUE_AT

Retrieves the measured value of a metric at a specific point in time. See above
for the time format.

*Format:*

    VALUE_AT [metric] [at]

*Response:*

    Numeric value as a string or "null"

*Example:*

     >> VALUE_AT my_application.response_time.avg-30 -3hours
     << 17.42

     >> VALUE_AT my_application.response_time.avg-30 now
     << null

<br />


#### VALUES_IN

Retrieves all measured values of a metric in a specific time interval. See above
for the time format.

*Format:*

    VALUE_AT [metric] [from] [until]

*Response:*

    white space seperated Timestamp:Value tuples or "null"

*Example:*

     >> VALUE_AT my_application.response_time.avg-30 -2hours now
     << 1360804571:4233.52 1360804581:4312.36 1360804591:6323.12

     >> VALUE_AT my_application.response_time.avg-30 -6hours -5hours
     << null



