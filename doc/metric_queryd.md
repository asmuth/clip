The metric-queryd server
========================

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


### Example

This will start `metric-queryd` in foreground, listen on HTTP port 8080, connect
to a backend MySQL server on `localhost` and use the database `mydb`:

    $ metric-queryd --listen_http localhost:8080 --backend 'mysql://localhost:3306/mydb?user=root'

Now, open your browser and copy this url:

    http://localhost:8080/embed/timeseries?....

metric-queryd will execute this SQL query and return a timeseries chart of the
result data:


    SELECT ...;

Asumming you have a table `host_stats` with some rows in your database, the result
could look something like this.


### Widgets

`metric-queryd` supports a number of other widgets:
