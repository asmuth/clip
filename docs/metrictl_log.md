The metrictl log command
=========================

The metrictl log command allows you to retrieve a logfile of collected
measurements for a specific metric.

    Usage: $ metricctl log [<options>] <metric>
      --from <timespec>         Set the start time (inclusive)
      --until <timespec>        Set the end time (exclusive)
      -x, --filter <expr>       Filter the returned metric instances by label.

    Examples:
      $ metrictl log load_average
      $ metrictl log load_average -x 'hostname=localhost'
