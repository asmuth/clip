Metric Types
============

This page discusses the different metric types. It is important that you choose
the correct metric type for your metrics. You can set metric's type using th
`kind` stanza - for example:

    metric my_metric {
      kind gauge(float64)
    }

These are the available metric types:

<table>
  <thead>
    <tr>
      <th>Type</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>gauge(T)</strong></code></td>
      <td>Collect point-in-time measurements</td>
    </tr>
    <tr>
      <td><code><strong>monotonic(T)</strong></code></td>
      <td>Collect monotonically increasing values (recommended where applicable)</td>
    </tr>
    <tr>
      <td><code><strong>counter(T)</strong></code></td>
      <td>Collect incremental counter updates</td>
    </tr>
    <tr>
      <td><code><strong>string</strong></code></td>
      <td>Collect UTF-8 strings</td>
    </tr>
  </tbody>
</table>


In the table above, `T` refers to a primitive data type. Since metric-collectd
supports a number of pluggable backend databases, it needs map all primitive data
types from an internal set of types to the native types of the backend database.

This is the list of valid primitive data types. Please check the backend-specific
documentation for more detailed information to which types they map in the backend:

<table>
  <thead>
    <tr>
      <th>Type</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>uint64</strong></code></td>
      <td>Stores a 64-bit unsigned integer</td>
    </tr>
    <tr>
      <td><code><strong>int64</strong></code></td>
      <td>Stores a 64-bit signed integer</td>
    </tr>
    <tr>
      <td><code><strong>float64</strong></code></td>
      <td>Stores a 64-bit floating point value.</td>
    </tr>
  </tbody>
</table>


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


