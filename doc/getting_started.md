Getting Started
===============

_This guide will walk you through configuring a custom metric, inserting
measurements and drawing charts from the data. If you do not have installed
metrictools yet, read the [Installation page](/documentation/installation) first._

When you installed metrictools, two main programs were added to your system:
`metrictl` and `metricd`. The `metrictl` program is a command line interface
similar to `git` that allows you to manage and query metric data and create charts.

The second `metricd` program is a background server that exposes a HTTP API
which allows you to execute the same commands that you can execute with `metrictl`.
Note that running `metricd` is entirely optional and for now we're only interested
in the `metrictl` program. 

### Adding a metric

In contrast to some other systems, metrictools requires all metrics to be explicitly
configured in a configuration file. You can find some discussion on why we think
this is a good idea on the Data Model page. To get started we need to create a
simple configuration file. Save this to `~/.metrictl.conf`

    backend "sqlite:///tmp/test.sqlite"

    metric temperature {
      kind gauge(float64)
    }

The first `backend` stanza configures the storage backend. metrictools does
not implement its own storage backend, but uses a pluggable external backend.
For our example we will use a sqlite database stored in `/tmp/test.sqlite` as
the storage backend.

The second `metric` block adds a new metric called `temperature` that we will
use for our example. We have to specify a metric type, which we will simply set
to `gauge(float64)` for now (have a look at the Metric Types page for more
information)

With the config file in place, we can start adding data to our new metric. Run
this command to store `23.5` as the current temperature value.

    $ metrictl insert temperature 23.5

Now we can run the `log` command to display a list of all metrics and their
latest values. We currently only have one metric, so the command will only
return a single result:

    $ metrictl list
    temperature                                             23.5

To see the historical values for a metric, use the `log` command. The `--from`
and `--until` flags are optional and included here for illustration purposes:

    $ metricctl log temperature --from "-30min" --until now
    2017-05-01 16:00:00 UTC                                 23.5
    2017-05-01 16:00:00 UTC                                 23.5
    2017-05-01 16:00:00 UTC                                 23.5
    2017-05-01 16:00:00 UTC                                 23.5
    ...

Depending on how many times you executed the `insert` command, the `log` command
should return one or more lines. Before continuing with the next step, execute
the insert command a couple of times and make sure there are at least two data
points.

Using the command line below we will create a simple line chart with one series.
Note that the `plot` command is reasonably powerful and accepts a large number of
options not discussed here.

    $ metricctl plot temperature --from "-30min" --until now --output /tmp/temperature_chart.png

If you open the `/tmp/temperature_chart.png` in the image viewer of your choice,
you should see something like this:

<img src="/examples/linecharts/simple_lines/simple_lines.png" style="width: 100%;">


### Adding a unit of measurement

What good is a measurement if we don't know the unit it's reported in? metrictools
encourages you to put this information into the configuration of a metric. The
configuration of the `celsius` unit below should be self-explanatory.

    unit celsius {
      unit_name celsius 1 "Celsius" "Celcius" "°C"
    }

    metric temperature {
      kind gauge(float64)
      unit celsius
    }

Once the unit is configured, all the commands and plots will display it by default.
For detailed information on the unit configuration please refer to the Units of
measurement page.

    $ metrictl list
    temperature                                          23.5 °C


### Adding labels

For the sake of the example, let's suppose we wanted to measure the temperature
of three devices `deviceA`, `deviceB` and `deviceC`. Using the knowledge we
already have, we could simply create three metrics, for example 
`temperature.deviceA`, `temperature.deviceB` and so on.

However, this would be less than ideal for two reasons. First of all we would
have to duplicate the metric configuration for each instance of the metric. This
might be workable for three devices but is clearly not an option if we want
to measure the temperature of a dynamic (and potentially large) list of devices.

The second, more subtle, problem would be the mixing of data and schema in the
metric name. Just looking at a compound metric name like `temperature.deviceA` doesn't
tell us which part of the string describes what is being displayed (the temperature)
and which part of the string contains variable data (deviceA). This might not
seem significant but gets hairy once you want to enumerate ("walk") the devices
or have a lot of metrics from different sources.

The proper way to create many instances of the same metric is using labels.
The concept is tricky to explain in technical terms but intuitive, so try to
read this section until the end and see if it makes sense:

To allow instancing of the same metric, each metric can have one or more labels
such as `device\_id` or `hostname`. Each unique combination of label values
then references an instance of the metric. For example, let's say we add a label
`device_id` to our temperature metric:

    metric temperature {
      kind gauge(float64)
      unit celsius
      labels device_id
    }

Now, each `device_id` value will have it's own metric instance. Let's insert
some example data:

    $ metrictl insert device_id=deviceA temperature 23.5
    $ metrictl insert device_id=deviceB temperature 22.1
    $ metrictl insert device_id=deviceC temperature 28.7

If we run the `list` command again, we can see that there are now three
instances of the temperature metric:

    $ metrictl list
    temperature
      deviceA                                            23.5 °C
      deviceB                                            22.1 °C
      deviceC                                            28.7 °C

Let's display all three devices in a single chart:

    $ metricctl plot temperature --from "-30min" --until now --output /tmp/temperature_chart.png

The command should result in a plot similar to the one below. Have a look at the
page of the `plot` command for  more information on how to filter and aggregate
metrics.

<img src="/examples/linecharts/simple_lines/simple_lines.png" style="width: 100%;">


### Automating data collection

### Building a dashboard from the data

### Using predefined metrics from plugins

### Adding global labels


