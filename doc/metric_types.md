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

However, both of these metric types require that you report exactly one absolute
value per metric instance, so if you want to report data from more than one source
you _must_ make sure each source reports the measurements with a unique combination
of labels. Thi way each series of consecutive measurements from a single data
source ends up in its own metric instance.

The `counter` type allows incremental updates into the same instance from multiple
data sources, but it does require a bunch of expensive statekeeping and limits
scalability, so it should be avoided if possible.

Let's have a look at a promblematic example to illustrate the point: Assume we
wanted to record the a `revenue` metric which records the total revenue of a
fictional lemonade stand company. Now, we have many point of sale (POS) terminals
from which we need to scrape data.

If we just created a single `revenue` metric and sent measurements from each
terminal we would be forced to user the `counter` type and have each terminal
send relative revenue measurements. For example the POS terminals could send
one packet per minute each which contains the delta of the revenue (i.e. new
revenue) since the last packet.

Alas, this approach would be somewhat expensive and error prone. The better 
solution would be to report a monotonically increasing integer of total revenue
on each point of sales terminal and then add a `pos_id` label to the metric so
that we get one timeseries of monotonically increasing integers per terminal.
Note that we're also adding a `rate` stanza since we're not actually interested
in the absolute value of the monotonically increasing revenue integer but the
rate at which it increases.

    metric revenue {
      kind monotonic(uint64)
      unit euro
      labels pos_id
      rate 1m
    }

Now, we will get one revenue rate timeseries per pos terminal. This approach
is much nicer because it lost packets don't result in incorrect/inconsistent
data. It also guarantees a constant write load on the backend data store
without requiring centralized statekeeping.

    $ metricctl log revenue
    pos_id="Haarlemmerplein"
        2017-05-01 16:00:00 UTC                                  8.55 €
        2017-05-01 16:01:00 UTC                                 23.10 €
        2017-05-01 16:01:00 UTC                                 18.40 €
        ...

    pos_id="Stationsplein"
        2017-05-01 16:00:00 UTC                                  8.55 €
        2017-05-01 16:01:00 UTC                                 23.10 €
        2017-05-01 16:01:00 UTC                                 18.40 €
        ...

To calculate the total revenue we can use the `combine` function:

    $ metricctl query ...
    2017-05-01 16:00:00 UTC                                  8.55 €
    2017-05-01 16:01:00 UTC                                 23.10 €
    2017-05-01 16:01:00 UTC                                 18.40 €
    ...


Metric Type: gauge
------------------

The `gauge` type is intended for collecting point-in-time measurements. In other
words, gauges are used to peridodically store a "snapshot" of the current absolute
value of a measurement. Each snapshot will be displayed as-is and no assumptions
are made about what happened between two snapshots.

Good examples for `gauge` metrics would be `temperature` or `number of open
file descriptors`. In more technical terms, gauges should be used when sampling
from cotinuous data.

For example:

    metric room_temperature {
      kind gauge(float64)
      unit celcius
    }

Insert data:

    $ metrictl insert room_temperature 19.4
    $ metrictl insert room_temperature 19.7
    $ metrictl insert room_temperature 19.2

Result:

    ...

Metric Type: monotonic
----------------------


Metric Type: counter
--------------------


