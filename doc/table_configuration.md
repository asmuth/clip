Table Configuration
===================

The table configuration allows you to specify one or more target tables into which
measurements should be stored. A table configuration in metricd-collectd corresponds
to a table in your backend SQL database. Each table has a number of columns. Columns can
either be "labels" or "measures".

  - **Labels** are used for columns that identify the row, like a series
    identifier or attributes. Good examples for labels are `hostname`, `device_id`
    or `sensor_location`.

  - **Measures** are used for columns that store numeric measurements. Good
    examples for measurements are `request_count`, `temperature` or `xxx`.
    Measure columns have a data type (like `uint64`) and an aggregation function
    (like `sum`, `max` or `latest`).

When sending samples to metric-collectd, not every submitted sample will result
in a write to the backend database. Instead, the measures from all samples will
be pre-aggregated for each label before being written into the database.

Accordingly, each table has an `interval` setting that controls the length of
the aggregation time window. The default aggregation window is 10 seconds.

At the end of every aggregation window, metric-collectd will execute one INSERT
statement for every unique combination of labels in the input samples received
during the time window. As a result of this, the number of rows written to the
backend database depends only on the aggregation windpw of a table and the
"cardinality" of the input data (i.e. the number of unique label values), but
not on the number of individual samples that are sent to metric-collectd.

A table may have zero, one or more label columns. If a table does not have any
label columns, metric-collectd will always insert exactly one row for every time
window in which data was received.

While this pre-aggregation scheme always results in a fixed rate-limit on the
number of backend writes, it does not necessarily imply a loss of precision or
accuracy. Some aggregation functions like `latest` or `max` will forward the
sample value to the database in the same precision in which it was received.


### Example

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


Now, if we were sending samples from only one host - so `datacenter` and `hostname`
would be the same in every input sample - metric-collectd would also insert exactly
one row every 10 seconds. The row's `load_avg` column would contain the largest
of all received `host_stats.load_avg` samples (since the aggregation function is `max`)
and the row's `request_count` column would contain the sum of all received
`host_stats.request_count` samples (since the aggregation function is `sum`).


    INSERT INTO host_stats (now(), "dc1", "rack8host2", 0.23, 42);

If we were reporting data from 2 datacenters with 1,000 servers each,
metric-collectd would insert 2,000 rows every 10 seconds as there would be
2,000 unique `(datacenter, hostname)` combinations. Accordingly, each row's
`load_avg` column would contain the largest of all received `host_stats.load_avg`
samples for that host and eachs row's `request_count` column would contain the
sum of all received `host_stats.request_count` samples for that host.


## Configuration Syntax

Each table is defined within a single `table` block:

    table <table_name> {
      # table config here
    }

This is the list of valid stanzas within the table block:

#### interval

The interval stanza controls the aggregation window for a table. There must not
be more than one interval stanza per table block.

    interval <value>

Valid values are any number (interpreted as seconds) or strings of the format
`N[s(((ec)ond)s)|m(((in)ute)s)|h((our)s)|d((ay)s)|w((eek)s)|y((ear)s)]`. For
example: `1s`, `30min` or `6hours`.

#### label

The label stanza adds a label column to the table. There may be any number of
label stanzas (including zero) in a table block.

    label <column_name> [<data_type>]

The second data type parameter is optional. If it is set the value must be
any of the valid data types below (e.g. `string` or `uint64`).

#### measure

The measure stanza adds a measurement column to the table. There may be any number
of measure stanzas in a table block.

    measure <column_name> <aggregation_fun>

The format of the aggregation function parameter is in the format `function(type)`,
for example `sum(uint64)` or `average(float64)` or `count_distinct(string)`

