Data Model
==========

The primary abstractions in metrictools are called "metrics" and "metric instances".
The metrictools allow you to store and manage a list of "metrics". Each metric
has a name, for example `load_average` and a number of other properties, such as
a unit of measurement and a data type.

However, the metric itself does not contain any data; instead it is a container
object for one or more "instances". Each instance in the metric is identified by
a set of labels and holds a simple timeseries, i.e. a list of `(time, value)`
data points that are recorded over time. The empty set of labels is valid too,
so a metric may have only a single instance with no labels.

Consider this example, but keep in mind that the `host` and `datacenter` labels
are in no way special. They could just as well be called `planet` and `galaxy`
or something else:


    Metric               Instance                         Time         Value

    load_average   ->    host=s01.prod.mycorp.com    ->   2017-01-01   1.32
                         datacenter=dc1                   2017-01-01   1.32
                                                          2017-01-01   1.32
                                                          2017-01-01   1.32

                   ->    host=s01.prod.mycorp.com    ->   2017-01-01   1.32
                         datacenter=dc2                   2017-01-01   1.32
                                                          2017-01-01   1.32
                                                          2017-01-01   1.32


In contrast to some other systems, metrictools requires all metrics to be explicitly
configured in a [configuration file](/documentation/configuration-file). Instances
on the other hand are created on first insert and do not have any individual
configuration beyond their labels. It is expected that all instances of a metric
store data of the same kind, for example when reporting the same measurement from
a number of sensors.

  - **Metrics** must be configured in the configuration file
  - **Metric Instances** belong to metrics and are automatically created when values are inserted into them

The reason behind this two-level split into metrics and instances is configurability:
We would like to control settings like granularity and units of measurement for
each timeseries individually. However if we're collecting data from a large number
of sensors we do not want to duplicate the configuration for each single instance.
Metrics allow us to define the common configuration once, for an arbitrary number
of instances of the same kind.

Grouping instances into metrics also results in much cleaner data organization:
We're separating the semantic type of data (the metric name) from the usually
machine-generated instance identifier (the set of labels).

### Collecting data

The input data is collected as individual data points called "measurements".
Measurements may be actively pushed to metricd or pulled/scraped from a remote
endpoint using one of the transports like [HTTP](/documentation/collect-data-via-http)
or [shell scripts](/documentation/collect-data-via-shell).

A measurements consists of a metric name, a list of labels, a value and a
timestamp. The timestamp is generated implicitly if it is not provided. For
example, to report the latest "load_average" value from 3 hosts, you would send
measurements similar to the ones below. For more information on the various text
formats and other encodings, visit the [input formats page](/documentation/sample-format).

    Measurement: load_average_avg {host="server1"} 0.42
    Measurement: load_average_avg {host="server2"} 3.32
    Measurement: load_average_avg {host="server3"} 1.23

It is worth mentioning that the exact semantics of how measurements are added to
the metric instances are dependant on the [metric type](/documentation/metric-types).
The difference in semantics becomes important once we start to send measurements
from multiple machines and/or run multiple metrictools instances using federation.

While a `counter` type is offered that allows incremental updates into the same
instance from multiple data sources it does require a bunch of expensive
statekeeping and should be avoided if possible.

The recommended way of reporting measurements is using the `monotonic` or `gauge`
types which require no statekeeping and have a number of other nice properties.
However, both of these metric types require that you report exactly one absolute
value per metric instance, so if you want to report data from more than one source
you _must_ make sure each source reports the measurements with a unique combination
of labels. For more information have a look at the [metric types page](/documentation/metric-types).

### Metric and Label Names

The metric name is chosen by the user and may consist of all alphanumeric
characters `A-Za-z0-9`, dots `.`, underscores `\_`, hyphens, `-` and slashes `/`.
Label names and values may consist of all alphanumeric characters `A-Za-z0-9`,
dots `.`, underscores `_`, hyphens, `-` and slashes `/`.  Label values may contain
any valid UTF-8. Note that the empty string is not a valid metric name.

Consider these examples of valid metric and label names:

    Metric Names:
        sys.cpu_util
        myapp.response_time
        /http/request_latency
        /apps/myapps/stats.daily_active_users
        ...

    Label Names:
        site
        device_id
        process_id
        disk_id
        hostname
        datacenter
        ...

Lastly, keep in mind that all transports allow you to rewrite metric names and
labels on the fly, so you can export metric names in a different format and
rewrite them on reception in metrictools.

