Collecting Metrics
==================

You can use FnordMetric Server to collect timeseries data (and subsequently
query the data using ChartSQL). FnordMetric Server stores the collected data
on local disk or in an external storage system (HBase).

To enable metric collection, you have to start fnordmetric-server with the 
`--storage_backend` flag. See backends for instructions how to configure each
backend.

### Metrics

Metrics are somewhat equivalent to tables in a regular SQL database. Each metric 
has a unique name and consists of a collection of data points called "samples"
that are recorded over time (i.e. a timeseries).

A "sample" is a single datapoint. Each sample contains at least a timestamp
and a numeric value. To keep the table analogy, each metric is a table that has
two default columns `value` and `time` and each sample is a row in that table.

You can query metrics using ChartSQL like normal tables:

    > select time, value from mymetric;

    ===================================
    | time                  |  value  |
    ===================================
    | 2014-11-08 20:30:12   |  0.913  |
    | 2014-11-08 20:30:42   |  0.837  |
    | 2014-11-08 20:31:13   |  0.638  |
    | 2014-11-08 20:31:41   |  0.326  |
    | ...                   |  ...    |


The metric name is chosen by the user and may consist of all alphanumeric
characters `A-Za-z0-9`, dots `.`, underscores `_`, hyphens, `-` and slashes `/`

#### Example metric names:

    sys.cpu_util
    myapp.response_time
    /http/request_latency
    /apps/myapps/stats.daily_active_users



### Metric Labels

To allow you to drill down into your metric data in arbitrary dimensions, each
sample can optionally be labelled with one or more "labels". Each label is a
key: value pair.

For example, you could record CPU utilization data into the metric `cpu_util`
from multiple hosts. You could attach a `hostname=...` and a `datacenter=...`
label to each sample in order to roll up CPU utilization aggregates by host,
datacenter or a combination of both.

    ...
    Sample: metric=cpu_util value=0.873 label[hostname]=machine64 label[datacenter]=ams1
    Sample: metric=cpu_util value=0.352 label[hostname]=machine65 label[datacenter]=ams1
    Sample: metric=cpu_util value=0.543 label[hostname]=machine66 label[datacenter]=ams1
    ...

When querying metrics with ChartSQL, the label keys act as table columns so you
can filter and aggregate/group by label values.

    > select time, value, hostname, datacenter from cpu_util;

    ==============================================================
    | time                  |  value  | hostanme   | datacenter  |
    ==============================================================
    | 2014-11-08 20:30:12   |  0.873  | machine64  | ams1        |
    | 2014-11-08 20:30:12   |  0.352  | machine65  | ams1        |
    | 2014-11-08 20:30:12   |  0.543  | machine66  | ams1        |
    | ...                   |  ...    | ...        | ...         |


Label keys and values may consist of all alphanumeric characters `A-Za-z0-9`,
dots `.`, underscores `_`, hyphens, `-` and slashes `/`


### Sending Metric Data

FnordMetric Server currently supports two ways to insert metric data:

  + [Send Metric Data via HTTP](/documentation/metricdb/http_interface)
  + [Send Metric Data via statsd](/documentation/metricdb/statsd_interface)
