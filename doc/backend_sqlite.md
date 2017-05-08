The SQLite Storage Backend
==========================

To use the SQLite backend, add the `backend` stanza to your configuration file:

    backend "sqlite:///path/to/database"

For example (this would store the data in `/var/volatile/metrics.sqlite`)

    backend "sqlite:///var/volate/metrics.sqlite"


SQLite Tables
-------------

Consider this example metric configuration:

    metric load_average {
      kind gauge(float64)
      labels datacenter, hostname
    }

When you start metricd with the configuration from above, it will create two
tables `load\_average:last` and `load\_average:history` in the backend database
that look similar to the ones from the SQL statements below.

    CREATE TABLE "load_average:last" (
      datacenter string,
      hostname string,
      value float64,
      UNIQUE KEY (datacenter, hostname)
    );

    CREATE TABLE "load_average:history" (
      time datetime,
      datacenter string,
      hostname string,
      value float64
    );

Note that a `time` columns was automatically added to the history table. The `time`
column stores the end of the time window in which the samples were

