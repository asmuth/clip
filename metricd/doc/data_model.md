Data Model
==========

In principle, metricd is a very simple timeseries database. The server stores a
list of "metrics". Each metric has a name, for example `load_average` and a
number of other configuration properties.

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


- usecase for series ids
- metrics configured, series created at runtime
- valid metric and series ids

##### Example metric names:

    sys.cpu_util
    myapp.response_time
    /http/request_latency
    /apps/myapps/stats.daily_active_users



### Internals

Since metricd stores each series individually as a btree-like structure on disk,
it supports the following operations:

   -
