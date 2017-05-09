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

## Adding a metric

In contrast to some other systems, metrictools requires all metrics to be explicitly
configured in a [configuration file](/documentation/configuration-file). You can
find some discussion on why we think this is a good idea on the
[data model](/documentation/data-model) page. To get started we need to create a
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
to `gauge(float64)` for now (have a look at the [metric types](/documentation/metric-types)
page for more information)

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

    $ metricctl plot --metric temperature --from "-30min" --until now --output /tmp/temperature_chart.png

If you open the `/tmp/temperature_chart.png` in the image viewer of your choice,
you should see something like this:

<img src="/examples/linecharts/simple_lines/simple_lines.png" style="width: 100%;">


## Adding a unit of measurement

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

For detailed information on the unit configuration please refer to the [units of
measurement](/documentatio/units-of-measurement) page. Once the unit is configured,
all the commands and plots will display it by default &mdash; much better now:

    $ metrictl list
    temperature                                          23.5 °C


## Adding labels

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

    $ metricctl plot --metric temperature --from "-30min" --until now --output /tmp/temperature_chart.png

The command should result in a plot similar to the one below. Have a look at the
[page of the `plot` command](/documentation/command-plot) for more information on
how to filter and aggregate metrics.

<img src="/examples/linecharts/simple_lines/simple_lines.png" style="width: 100%;">


## Automating data collection

While our previous approach of explicitly adding values to the metric using
the `insert` command is fine and we could continue doing it this way, let's try
another method for demonstration purposes:

metrictools can also actively collect data through a number of transports. For
example, you can pull data from any HTTP endpoint using the
[`collect\_http`](/documentation/collect-data-via-http) block.
Or you can fetch data via SNMP using the [`collect\_snmp`](/documentation/collect-data-via-snmp)
block.

For this example, let's use the [`collect\_proc`](/documentation/collect-data-via-shell)
block. This block will periodically execute any programm (for example, a shell script)
and read measurements from the programs standard output.

Save this script to `~/collect_temp.sh` and set the executable bit. (The script
should be in the same folder as the `metrictl.conf` file)

    #!/bin/bash
    echo 'temperature {device="deviceA"} 23.5'
    echo 'temperature {device="deviceB"} 22.1'
    echo 'temperature {device="deviceC"} 28.7'

When you execute the script it wil simply print three measurements in the
Borgmon/Prometheus text format. To tell metrictools to execute the script every
10s and store the output, add this block to the configuration file:

    collect_proc {
      cmd "collect_temp.sh"
      format borgmon
      interval 10s
    }

Then simply run the `collect` command to start the collection process. Let's
also add the `--verbose` switch so that we see what's going on

    $ metrictl collect --verbose


## Building a dashboard from the data

All metrictool commands can also be executed through a HTTP API. This allows you
to plug the data and charts into pretty much any HTML website (or other app).

To access the HTTP API we have to start the `metricd` program. The metricd server
will also automatically run the collect command. For more information on
running metricd, have a look at [the metricd page](/documentation/metricd).
Execute this command line to start metricd in another tab:

    $ metricd --config ~/.metrictl.conf --listen_http localhost:8080

If we run the `metrictl plot` command with the `--format url` flag, it will
return a url representation of the command. However as you can probably see
the mapping of command line arguments to url parameters is trivial.

    $ metricctl plot --metric temperature --from "-30min" --until now --format url
    /api/v1/plot?metric=temperature&from=-30min&until=now

As a simple demonstration, we will create a standalone html page that embeds
a chart using an iframe. Note that there are other ways to include charts and
data in the page (besides iframes). Save this to `temp_dashboard.html` and open
it in your browser:

    <!DOCTYPE html>
    <title>Demo Dashboard</title>

    <iframe src="http://localhost:8080/api/v1/plot?metric=temperature&from=-30min&until=now">

The page should look like this:

  <img src="/img/fnordmetric_ui_example_screen.png" style="width: 100%;">


## Using predefined metrics from plugins

We've seen how we can configure and display our own custom metrics. Lastly let's
see how we can use or create prebuilt plugins. A metrictools plugin consists
of a configuration file (like the one we wrote, but without the `backend` stanza)
and optionally a list of other files such as the exporter/collection programs
or html dashboards.

Assuming you're running this on a linux machine and have installed metrictools
in your system using the default prefix, you should be able to add [the linux
plugin](/plugins/linux) to your config with one line:

    include "${plugindir}/linux/linux-plugin.conf"

Adding this line to your config will add a long list of metrics (all prefixed
with `linux.`) as well as a couple of simple dashboards.

By default, most of the linux plugin metrics do not contain any labels. However
since we might want to combine metrics from more than one machine using Federation
it seems like a good idea to add a global ``hostname` label to _all_ metrics and
set a default value for the label. This can be achieved by adding these two
lines to the config file:

    labels hostname
    label_set hostname localhost default

Note that hostname is an opaque string in this case. You could also set more than
one and different default labels. For example `datacenter, rack, machine`

Let's restart our metricd instance and see what our `metrictl list` looks like:

    $ metrictl list
    temperature                                             23.5
    temperature                                             23.5
    temperature                                             23.5
    temperature                                             23.5
    temperature                                             23.5
    temperature                                             23.5

If you open your web browser and navigate to `http://localhost:8080/plugin/linux`
you should also be able to see a default dashboard that is included in the linux
plugin:

  <img src="/img/fnordmetric_ui_example_screen.png" style="width: 100%;">

---

_If you've read this far and want to learn more, please have a look at the
[Data Model](/documentation/data-model), [Metric Types](/documentation/metric-types)
and [Federation](/documentation/federation) pages.
