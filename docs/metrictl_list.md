The metrictl list command
=========================

The metrictl list command allows you to list all metrics and their instances
and latest values.

    Usage: $ metricctl list [<options>] [<metric>]
      -x, --filter <expr>       Filter the returned metric instances by label.

    Examples:
      $ metrictl list
      $ metrictl list load_average -x 'hostname=localhost'
