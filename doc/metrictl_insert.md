The metrictl insert command
===========================

The metrictl insert command allows you to add a measurement to a metric.

    Usage: $ metricctl insert [<options>] <metric> [<label>=<label>]... <value>

    Examples:
      $ metrictl insert -m users_online -v 523562
      $ metrictl insert -m load_average -t hostname=localhost -v 0.52
