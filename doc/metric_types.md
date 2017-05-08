Metric Types
============

This page discusses the different metric types. It is important that you choose
the correct metric type for your metrics. You can set a metric's type using the
`kind` stanza. For example:

    metric my_metric {
      kind gauge(float64)
    }

These are the available metric types:

<table>
  <thead>
    <tr>
      <th style="width: 26%;">Type</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>
        <code><strong>gauge(float64)</strong></code><br />
        <code><strong>gauge(uint64)</strong></code><br />
        <code><strong>gauge(int64)</strong></code>
      </td>
      <td>Collect point-in-time measurements</td>
    </tr>
    <tr>
      <td>
        <code><strong>monotonic(float64)</strong></code><br />
        <code><strong>monotonic(uint64)</strong></code>
      </td>
      <td>Collect monotonically increasing values (recommended where applicable)</td>
    </tr>
    <tr>
      <td>
        <code><strong>counter(float64)</strong></code><br />
        <code><strong>counter(uint64)</strong></code><br />
        <code><strong>counter(int64)</strong></code>
      </td>
      <td>Collect incremental counter updates</td>
    </tr>
    <tr>
      <td><code><strong>string</strong></code></td>
      <td>Collect UTF-8 strings</td>
    </tr>
  </tbody>
</table>

Since metrictools supports a number of pluggable backend databases, it needs to
map all primitive data types from an internal set of types to the native types
of the backend database. Please check the backend-specific documentation for
more detailed information to which types they map in the backend:

#### When to use gauge/monotonic vs counter?

The main difference between gauge/monotonic metrics and counters are that the
former require you to report a absolute value for each metric instance while
the latter one allows you to send incremental updates.

Still, the recommended way of reporting measurements is using the `monotonic` or
`gauge` types since they require no statekeeping in metrictools and have a number
of other nice properties.

Both of thes `gauge` and `monotonic` metric types require that you report exactly
one absolute value per metric instance, so if you want to report data from more
than one source you _must_ make sure each source reports the measurements with
a unique combination of labels. Thi way each series of consecutive measurements
from a single data source ends up in its own metric instance.

The `counter` type allows incremental updates into the same instance from multiple
data sources, but it does require a bunch of expensive statekeeping and limits
scalability, so it should be avoided if possible.

#### Example

Let's have a look at a promblematic example to illustrate the point: Assume we
wanted to record a `revenue` metric which records the total revenue of a
fictional lemonade stand company. Now, we have many point of sale (POS) terminals
from which we need to scrape data.

If we just created a single `revenue` metric and sent measurements from each
terminal we would be forced to use the `counter` type and have each terminal
send relative revenue measurements. For example the POS terminals could send
one packet per minute each which contains the delta of the revenue (i.e. new
revenue) since the last packet. This is what you might be used to doing from
other systems such as StatsD/graphite.

Alas, the "incremental relative updates" approach is somewhat expensive and error
prone. The better solution would is to report a monotonically increasing integer
of total revenue on each point of sales terminal and then add a `pos_id` label to
the metric so that we get one timeseries of monotonically increasing integers per
terminal. Note that we're also adding a `rate` stanza since we're not actually
interested in the absolute value of the "total revenue" integer but the rate at
which it increases.

    metric revenue {
      kind monotonic(uint64)
      unit euro
      labels pos_id
      rate 1m
    }

Now, we will get one timeseries of "Revenue per 1 minute" per POS terminal. This
approach is much nicer because it lost packets don't result in incorrect/inconsistent
data. It also guarantees a constant write load on the backend data store
without requiring centralized statekeeping.

    $ metricctl log revenue
    pos_id="Haarlemmerplein"
        2017-05-01 16:00:00 UTC                             8.55 €/min
        2017-05-01 16:01:00 UTC                            23.10 €/min
        2017-05-01 16:02:00 UTC                            18.40 €/min
        ...

    pos_id="Stationsplein"
        2017-05-01 16:00:00 UTC                             8.55 €/min
        2017-05-01 16:01:00 UTC                            23.10 €/min
        2017-05-01 16:02:00 UTC                            18.40 €/min
        ...

To calculate the total revenue we can use the `combine` function:

    $ metricctl query ...
    2017-05-01 16:00:00 UTC                                 8.55 €/min
    2017-05-01 16:01:00 UTC                                23.10 €/min
    2017-05-01 16:02:00 UTC                                18.40 €/min
    ...


Metric Type: gauge
------------------

The `gauge` type is intended for collecting point-in-time measurements. In other
words, gauges are used to peridodically store a "snapshot" of the current absolute
value of a measurement. Each snapshot will be displayed as-is and no assumptions
are made about what happened between two snapshots.

Good examples for `gauge` metrics would be `temperature` or `number of open
file descriptors`. In more technical terms, gauges should be used when sampling
from continuous data.

Note that gauges require that each measurement contains the current _absolute_ value.
If you want to report data from more than one source you _must_ make sure each
source reports the measurements with a unique combination of labels.

Consider this example of a `gauge` metric:

    metric room_temperature {
      kind gauge(float64)
      unit celcius
    }

    # Insert data
    $ metrictl insert room_temperature 19.4
    $ metrictl insert room_temperature 19.7
    $ metrictl insert room_temperature 19.2

    # Result
    $ metricctl log room_temperature
    2017-05-01 16:00:01 UTC                              19.4 °C
    2017-05-01 16:00:02 UTC                              19.7 °C
    2017-05-01 16:00:03 UTC                              19.2 °C


Metric Type: monotonic
----------------------

The `monotonic` type is similar to the `gauge` type but expects that the reported
value is a monotonically increasing number. The reason for choosing a monotonic
metric over a gauge whenever possible is that with monotonic gauges we can make
assumptions about what happened between two samples of the value.

Still, the number is allowed to "wrap around" to zero. So you could, e.g. store
the value of a monotonic counter in the memory of a process and simply reset
to zero when the process restarts.

Monotonic metrics are most useful in combination with the `rate` stanza, which
will calculate the rate over a given time window from the monotonically increasing
value.

Note that monotonic metrics require that each measurement contains the current
_absolute_ value. If you want to report data from more than one source you
_must_ make sure each source reports the measurements with a unique combination
of labels.

Consider this example of a `monotonic` metric:

    metric number_of_context_switches {
      kind monotonic(uint64)
      rate 1s
    }

    # Insert data
    $ metrictl insert number_of_context_switches 10
    $ metrictl insert number_of_context_switches 252
    $ metrictl insert number_of_context_switches 9052
    $ metrictl insert number_of_context_switches 9063
    $ metrictl insert number_of_context_switches 9090

    # Result
    $ metricctl log number_of_context_switches
    2017-05-01 16:00:01 UTC                              52/s
    2017-05-01 16:00:02 UTC                              64/s
    2017-05-01 16:00:03 UTC                           20434/s
    2017-05-01 16:00:04 UTC                              12/s
    2017-05-01 16:00:05 UTC                              23/s


Metric Type: counter
--------------------

The `counter` type is a simple incremental counter. Each measurement is expected
to be a relative update. The counter type requires a rate stanza. Usage of the
counter type should be avoided where possible since it is less scalable and
reliable than the other types (see above).

Note that when reporting measurements for a counter metric, you can send the
measurements from more than one data sources and in any order as the underyling
"add" operation is commutative.

Consider this example of a `counter` metric:

    metric number_of_signups {
      kind counter(uint64)
      rate 1m
    }

    # Insert data
    $ metrictl insert number_of_signups 1
    $ metrictl insert number_of_signups 1
    $ metrictl insert number_of_signups 1
    $ metrictl insert number_of_signups 1
      ...

    # Result
    $ metricctl log number_of_signups
    2017-05-01 16:00:01 UTC                             523/min
    2017-05-01 16:00:01 UTC                             645/min
    2017-05-01 16:00:01 UTC                             815/min
    2017-05-01 16:00:01 UTC                            1256/min
