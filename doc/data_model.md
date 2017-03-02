Data Model
===========

metric-collectd collects measurements in a "key:value"-style format. The collected
measurements are aggregated and then stored into tables in your backend SQL database.

Each table into which measurements should be written has a corresponding configuration
entry in the `metric-collectd` configuration file. The table configuration contains
a list of columns for each table. Columns can either be "labels" or "measures".

  - **Labels** are used for columns that identify the row, like a series
    identifier or attributes. Good examples for labels are `hostname`, `device_id`
    or `sensor_location`.

  - **Measures** are used for columns that store numeric measurements. Good
    examples for measurements are `request_count`, `temperature` or `xxx`.
    Measure columns have a data type (like `uint64`) and an aggregation function
    (like `sum`, `max` or `latest`).

Consider this example table configuration:

    create_tables on

    table host_stats {
      interval 10s
      label datacenter
      label hostname
      measure request_count counter(uint64)
      measure load_avg max(float64)
    }

When you start metric-collectd with the configuration from above, it will create
a table `host_stats` in the backend database that looks similar to the one from
the SQL statement below. Note that metric-collectd will automatically add a
`time` column to the table which stores the time window in which the samples
were collected.

    CREATE TABLE host_stats (
      time datetime,
      datacenter string,
      hostname string,
      load_avg float64,
      request_count uint64,
      PRIMARY KEY (time, datacenter, hostname)
    );



Now, the input measurements are collected as individual data points called "samples".
Samples may be actively pushed to metric-collectd or pulled/scraped from a remote endpoint
using one of the transport protocol.

A sample consists of a "metric name", a value and a timestamp. The timestamp is
generated implicitly if it is not provided.

The metric name is used to identify the table and column ("measure") into which
the data should be written. The general format is `table.column` (i.e. first
the table name, then a dot, then the column name).

For example, to report the measure "load_avg" in the table "host_stats", you
would send samples similar to these:

    ...
    Sample: metric=host_stats.load_avg value=0.42
    Sample: metric=host_stats.load_avg value=0.23
    ...

The later, you could query the `host_stats` table and retrieve the aggregated
values:

    > select time, load_avg from host_stats;

    =====================================
    | time                  | load_avg  |
    =====================================
    | 2014-11-08 20:30:40   | 0.913     |
    | 2014-11-08 20:30:50   | 0.837     |
    | 2014-11-08 20:31:00   | 0.638     |
    | 2014-11-08 20:31:10   | 0.326     |
    | ...                   | ...       |

To allow you to drill down into your measurements in arbitrary dimensions, each
sample can optionally be labelled with one or more "labels". Each label is a
key=value pair.

For example, you could record CPU utilization data into the table `host_stats`
from multiple hosts. You could add a `hostname=...` and a `datacenter=...`
label to each sample in order to roll up CPU utilization aggregates by host,
datacenter or a combination of both.

    ...
    Sample: metric=cpu_util value=0.873 label[hostname]=machine64 label[datacenter]=ams1
    Sample: metric=cpu_util value=0.352 label[hostname]=machine65 label[datacenter]=ams1
    Sample: metric=cpu_util value=0.543 label[hostname]=machine66 label[datacenter]=ams1
    ...

The labels correspond to columns in the target SQL table, so you can filter and
aggregate/group by label values.

    > select time, hostname, datacenter, load_avg from cpu_util;

    ================================================================
    | time                  | hostname   | datacenter  | load_avg  |
    ================================================================
    | 2014-11-08 20:30:12   | machine64  | ams1        | 0.913     |
    | 2014-11-08 20:30:12   | machine65  | ams1        | 0.837     |
    | 2014-11-08 20:30:12   | machine66  | ams1        | 0.638     |
    | ...                   | ...        | ...         | ...       |

Of course, you can also combine multiple measurements in a single table, so
for example we could report CPU utilization and number of requests per server in
a single table:

    Sample: metric=host_stats.load_avg value=0.42
    Sample: metric=host_stats.request_count value=23

And then query the data using SQL:

    > select time, hostname, load_avg, request_count from cpu_util;

    ==================================================================
    | time                  | hostname   | load_avg  | request_count |
    ==================================================================
    | 2014-11-08 20:30:12   | machine64  | 0.913     | 52362         |
    | 2014-11-08 20:30:12   | machine65  | 0.837     | 52352         |
    | 2014-11-08 20:30:12   | machine66  | 0.638     | 63463         |
    | ...                   | ...        | ...       | ...           |


When sending samples to metric-collectd, not every submitted sample will result
in a write to the backend database. Instead, the values from all samples will
be aggregated for each label before being written into the database. Accordingly,
each table has an `interval` setting that controls the length of the aggregation
time window. The default aggregation window is 10 seconds.

For example, if we were sending samples from one to the example table from above
- so `datacenter` and `hostname` would be the same in every input sample -
metric-collectd would also insert exactly one row every 10 seconds. The row's
`load_avg` column would contain the largest of all received `host_stats.load_avg`
samples (since the aggregation function is `max`) and the row's `request_count`
column would contain the sum of all received `host_stats.request_count` samples
(since the aggregation function is `sum`).

    INSERT INTO host_stats (now(), "dc1", "rack8host2", 0.23, 42);

If we were reporting data from 2 datacenters with 1,000 servers each,
metric-collectd would insert 2,000 rows every 10 seconds as there would be
2,000 unique `(datacenter, hostname)` combinations. Accordingly, each row's
`load_avg` column would contain the largest of all received `host_stats.load_avg`
samples for that host and eachs row's `request_count` column would contain the
sum of all received `host_stats.request_count` samples for that host.

## Aggregation

At the end of every aggregation window, metric-collectd will execute one INSERT
statement for every unique combination of labels in the input samples received
during the time window. The values for all measure columns will be computed 
by applying the aggregation function for that column to all received input
samples for the column.

As a result of this, the number of rows written to the backend database depends
only on the aggregation windpw of a table and the "cardinality" of the input data
(i.e. the number of unique label values), but not on the number of individual
samples that are sent to metric-collectd.

A table may have zero, one or more label columns. If a table does not have any
label columns, metric-collectd will always insert exactly one row for every time
window in which data was received.

While this pre-aggregation scheme always results in a fixed rate-limit on the
number of backend writes, it does not necessarily imply a loss of precision or
accuracy. Some aggregation functions like `latest` or `max` will forward the
sample value to the database in the same precision in which it was received.

## Metric Name Format

The metric name is chosen by the user and may consist of all alphanumeric
characters `A-Za-z0-9`, dots `.`, underscores `_`, hyphens, `-` and slashes `/`.

Label keys and values may consist of all alphanumeric characters `A-Za-z0-9`,
dots `.`, underscores `_`, hyphens, `-` and slashes `/`

Note that all transports allow you to rewrite metric names on the fly, so you
can report metric names in a different format and rewrite them on reception in
metric-collectd.

