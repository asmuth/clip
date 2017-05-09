The metrictl plot command
=========================

The metrictl plot command allows you to draw a chart from one or more metrics.

    Usage: $ metricctl plot [<options>] [<metric>]
      --from <timespec>         Set the start time (inclusive)
      --until <timespec>        Set the end time (exclusive)
      -m, --metric <name>       Add a metric to plot
      -x, --filter <expr>       Filter series in the last metric.

    Examples:
      $ metrictl plot -m linux.load_avg_1m --from -2h --until now
      $ metrictl plot -m linux.load_avg_1m -x hostname=localhost

