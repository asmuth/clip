metrictools eventql plugin
==========================

### Simple Usage (EventQL running on localhost)

To use this plugin, add this to your `metrictl.conf`:

    include "${plugindir}/eventql/eventql-plugin.conf"

    collect_http {
      url "http://localhost:9175/eventql/stats"
      interval 10s
      format statsd
    }

