The metric-collectd server
==========================

metric-collectd is a standalone daemon process that collects measurements,
pre-aggregates them and stores the aggregated tuples into a backend database.

    Usage: $ metric-collectd [OPTIONS]
       -c, --config <file>       Load config file
       --daemonize               Daemonize the server
       --pidfile <file>          Write a PID file
       --loglevel <level>        Minimum log level (default: INFO)
       --[no]log_to_syslog       Do[n't] log to syslog
       --[no]log_to_stderr       Do[n't] log to stderr
       -?, --help                Display this help text and exit
       -v, --version             Display the version of this binary and exit

    Examples:
       $ metric-collectd -c metrics.conf
       $ metric-collectd -c metrics.conf --daemonize --log_to_syslog


On startup `metricd-collect` loads a config file that specifies the tables into
which collected measurements should be stored and from where the metrics should
be collected. For detailed information on the configuration syntax and semantics
please have a look at the [Data Model](/documentation/data-model) and
[Configuration File](/documentation/configuration-file) pages

### Example

Save this to `metric_collect.conf`

    # store collected metrics in mysql on localhost, database "metricdb"
    backend "mysql://localhost:3306/metricdb?user=root"

    # automatically create tables that don't exist
    create_tables on

    # collect measurements in the "host_stats" table. Insert one row per host every 10 seconds
    table host_stats {
      interval 10s
      label hostname
      measure load_avg max(float64)
      measure request_count sum(uint64)
    }

    # listen on UDP port 8125 for statsd packets
    listen_udp {
      port 8125
      format statsd
    }

Start `metric-collectd` in foreground:

    $ metric-collectd -c metric_collect.conf

Send measurements via UDP using the `netcat` utility:

    $ echo "host_stats.load_avg{hostname=localhost}:0.123" | nc -u localhost 8125
    $ echo "host_stats.request_count{hostname=localhost}:42" | nc -u localhost 8125

Query the data using SQL:

    > select time, hostname, load_avg, request_count from cpu_util;

    ==================================================================
    | time                  | hostname   | load_avg  | request_count |
    ==================================================================
    | 2014-11-08 20:30:12   | localhost  | 0.913     | 52362         |
    | 2014-11-08 20:30:12   | localhost  | 0.837     | 52352         |
    | 2014-11-08 20:30:12   | localhost  | 0.638     | 63463         |
    | ...                   | ...        | ...       | ...           |


