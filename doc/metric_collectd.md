metric-collectd
===============

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


### Config File

On startup `metricd-collect` loads a config file that specifies the tables into
which collected measurements should be stored and from where the metrics should
be collected. For detailed information on the configuration syntax and semantics
please have a look at the [Table Configuration page](/documentation/table_configration)

This is a list of all supported top-level stanzas in the `metric-collectd`
configuration file:

<table>
  <thead>
    <tr>
      <th>property</th>
      <th>description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>backend</strong></code></td>
      <td>Connection string/URL for the backend database (e.g. `mysql://localhost:3306/mydb?user=root`)</td>
    </tr>
    <tr>
      <td><code><strong>table</strong></code></td>
      <td>Declares a target table, i.e. a table into which measurements should be stored (A data sink). <a href="/documentation/table_configuration">More Information</a></td>
    </tr>
    <tr>
      <td><code><strong>create_tables</strong></code></td>
      <td>This settings controls wether tables should be automatically created (if they don't exist yet). Valid values are `on` and `off`</td>
    </tr>
    <tr>
      <td><code><strong>listen_udp</strong></code></td>
      <td>Declares a UDP/statsd listener port (A data source). <a href="/documentation/collect_via_udp">More Information</a></td>
    </tr>
    <tr>
      <td><code><strong>listen_http</strong></code></td>
      <td>Declares a HTTP listener port (A data source). <a href="/documentation/collect_via_http">More Information</a></td>
    </tr>
    <tr>
      <td><code><strong>collect_snmp</strong></code></td>
      <td>Pull data via SNMP (A data source). <a href="/documentation/collect_via_snmp">More Information</a></td>
    </tr>
    <tr>
      <td><code><strong>collect_http</strong></code></td>
      <td>Pull data via HTTP (A data source). <a href="/documentation/collect_via_http">More Information</a></td>
    </tr>
  </tbody>
</table>

### Example Config

Save this to `metric_collect.conf`

    # store collected metrics in mysql on localhost, database "metricdb"
    backend "mysql://localhost:3306/metricdb?user=root"

    # automatically create tables that don't exist
    create_tables on

    # collect measurements in the "host_stats" table. Insert one row per host every 10 seconds
    table host_stats {
      interval 10s
      field hostname string group
      field load_avg float64 max
      field request_count uint64 sum
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
