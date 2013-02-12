Plugins
-------

There are currently two plugins for FnordMetric Classic, they are included in
the gem and enabled by default.

### Gauge Explorer

The gauge explorer is a small toop to browse all existing gauges and preview their data.

<img src="/img/plugin_gauge_explorer_screen.png" width="630" class="shadow" />

The gauge explorer plugin is enabled by default, you can disable it by setting `enable_gauge_explorer` to `false` (more about Configuration in [Running FnordMetric](/documentation/classic_running_fm)):

    FnordMetric.config = {
      :enable_gauge_explorer => false
    }


### Active Users Plugin

The active users plugin shows all incoming events and a list of currently online users (read
more about how to submit usernames and picture in [Events and Gauges](/documentation/classic_event_handlers))

<img src="/img/plugin_active_users_screen.png" width="630" class="shadow" />

The active users plugin is enabled by default, you can disable it by setting `enable_active_users` to `false` (more about Configuration in [Running FnordMetric](/documentation/classic_running_fm)):

    FnordMetric.config = {
      :enable_active_users => false
    }
