Table Configuration
===================

The table configuration allows you to specify one or more target tables into which
measurements should be stored. A table configuration in metricd-collectd corresponds
to a table in your backend SQL database. Each table has a number of columns. Columns can
either be "labels" or "measures".

  - **Labels** are used for columns that identify the row, like a series
    identifier or attributes. Good examples for labels are `hostname`, `device_id`
    or `sensor_location`.

  - **Measures** are used for columns that store a numeric measurement. Good examples for
     measurements are `request_count`, `temperature` or `xxx`.


Additionally, each table has an `interval` setting that controls the length of
the aggregation time interval. The default aggregation interval is 10 seconds.

At the end of every aggregation interval, metric-collectd will execute one INSERT
statement for every unique combination of labels in the input samples received
during the aggregation interval. As a result of this, the number of rows written
to the backend database depends only on the aggregation interval of a table and
the "cardinality" of the input data (i.e. the number of unique label values),
but not on the number of individual samples that are sent to metric-collectd.

A table may have zero, one or more label columns. If a table does not have any
label columns, it will always insert exactly one row per interval in which data
was received.


### Example

Consider this example table configuration:

    create_tables on

    table host_stats {
      interval 10s;
      label    datacenter;
      label    hostname;
      measure  request_count  counter(uint64)
      measure  load_avg       max(float64)
    }

When you start metric-collectd with the configuration from above, it will create
a table `host_stats` in the backend database that looks similar to the one from
the SQL statement below. Note that metric-collectd will automatically add a
`time` column to the table which stores the time interval in which the samples
were collected.

    CREATE TABLE host_stats (
      time datetime,
      datacenter string,
      hostname string,
      load_avg float64,
      request_count uint64,
      PRIMARY KEY (time, datacenter, hostname)
    );


Now, if we were sending samples from only one host - so `datacenter` and `hostname`
would be the same in every input sample - metric-collectd would also insert exactly
one row every 10 seconds.

    INSERT INTO host_stats (now(), "dc1", "rack8host2", 0.23, 42);

If we were reporting data from 2 datacenters with 1,000 servers each,
metric-collectd would insert 2,000 rows every 10 seconds as there would be
2,000 unique `(datacenter, hostname)` combinations. Regardless of how many
individual input samples those hosts sent to metric-collectd.

### Configuration Syntax


### Data Types

### Aggregation Types

