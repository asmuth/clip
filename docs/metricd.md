The metricd server
==================

metricd is a standalone daemon process that collects measurements and exposes
a HTTP API that allows you to execute commands.


    Usage: $ metricd [OPTIONS]
       -c, --config <file>       Load config file
       --listen_http             Listen for HTTP connection on this address
       --daemonize               Daemonize the server
       --pidfile <file>          Write a PID file
       --loglevel <level>        Minimum log level (default: INFO)
       --[no]log_to_syslog       Do[n't] log to syslog
       --[no]log_to_stderr       Do[n't] log to stderr
       --datadir                 Override the default datadir (/usr/share/metrictools)
       --plugindir               Override the default plugindir (/usr/share/metrictools/plugins)
       -?, --help                Display this help text and exit
       -V, --version             Display the version of this binary and exit

    Examples:
       $ metricd -c /etc/metrictools/metricd.conf
       $ metricd -c /etc/metrictools/metricd.conf --daemonize --log_to_syslog

On startup `metricd` loads a config file. By default, the `metricd` program
will try to find the configuration file in the locations listed below in the
order in which they are listed. For detailed information on the configuration
syntax and semantics please have a look at the
[Data Model](/documentation/data-model) and
[Configuration File](/documentation/configuration-file) pages

    /etc/metrictools/metricd.conf
    /etc/metricd.conf

