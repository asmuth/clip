Data Model
==========

In principle, metricd is a very simple timeseries database. The server stores a
list of "metrics". Each metric has a name, for example `load_average` and a
number of other properties.

However, the metric itself does not contain any data; instead it is a container
object for one or more "series". Each series in the metric is identified by a
unique series ID and holds a simple timeseries, i.e. a list of `(time, value)`
tuples.

For Example:


    Metric               Series ID                      Time         Value

    load_average   ->    host01.prod.mycorp.com    ->   2017-01-01   1.32
                                                        2017-01-01   1.32
                                                        2017-01-01   1.32
                                                        2017-01-01   1.32

                   ->    host01.prod.mycorp.com    ->   2017-01-01   1.32
                                                        2017-01-01   1.32
                                                        2017-01-01   1.32
                                                        2017-01-01   1.32


Series are created on first insert and do not have any individual configuration
beyond their series id. It is expected that all series in a metric store data of
the same kind, for example when reporting the same measurement from a number of
sensors.

  - **Metrics** must be configured in the configuration file
  - **Series** are automatically created when values are inserted into them

The reason behind this two-level split into metrics and series is configurability:
We would like to control settings like granularity and units for each timeseries
individually. However if we're collecting data from a large number of sensors we
do not want to duplicate the configuration for each single series. Metrics allow
us to define the common configuration once, for an arbitrary number of series of
the same kind.

Grouping series into metrics also results in much cleaner data organization:
We're separating the semantic type of data (the metric name) from the usually
machine-generated series identifier (the series id).

##### Example metric names:

    sys.cpu_util
    myapp.response_time
    /http/request_latency
    /apps/myapps/stats.daily_active_users

Metric names can contain any alphanumeric characters as well as the dot, slash,
underscore and hyphen (`a-zA-Z0-9\./_-`). The series ID may contain any valid UTF-8.
Note that the empty string is a valid series id, but not a valid metric name.


### Internals

metricd stores each series individually as a btree-like structure on disk and
keeps the list of series within a metric as a map in memory. It hence supports
the following operations:

   - Fast listing of metrics
   - Fast listing of series within a metric
   - Fast lookups and range scans on individual series (log(N) where N=number of values in the series)
   - Fast inserts into individual series (log(N) where N=number of existing values in the series)
   - Summarization of all series in a metric is O(N) where N=total number of values in all series

