metric-queryd
=============

metric-queryd is a simple stateless service that executes SQL queries on a backend
database and returns the results as charts.

    Usage: $ metric-queryd [OPTIONS]

       --backend                 Set the backend database connection URI/string
       --listen_http             Listen for HTTP connection on this address
       --daemonize               Daemonize the server
       --pidfile <file>          Write a PID file
       --loglevel <level>        Minimum log level (default: INFO)
       --[no]log_to_syslog       Do[n't] log to syslog
       --[no]log_to_stderr       Do[n't] log to stderr
       -?, --help                Display this help text and exit
       -v, --version             Display the version of this binary and exit

    Examples:
       $ metric-queryd --backend "mysql://localhost:3306/mydb?user=root" --listen_http 0.0.0.0:8080


