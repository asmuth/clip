Federation
==========

A common scenario is that you want to collect metrics from multiple machines or
devices and  consolidate them in a single database for querying and graphing.
metrictools provides two principal ways to achieve this:

### Run a single metricd instance

The most simple way to collect data from multiple machines is to run a single
metricd instance and use for example the http transport to collect data from
other machines.

For example, let's assume we have three web servers `web01` .. `web03` and every
one of them exposes a `/stats` http endpoint that exports the current value
of the `number\_of\_requests\_processed` metric. We could use the following configuration
snippet to collect the metric from each of the webservers:

    backend "sqlite:///var/tmp/metrics.sqlite"

    metric number_of_requests_processed {
      kind monotonic(uint64)
      rate 1s
      labels server_id
    }

    collect_http {
      url "http://web01/stats"
      interval 10s
      label_set server_id "web01"
    }

    collect_http {
      url "http://web02/stats"
      interval 10s
      label_set server_id "web02"
    }

    collect_http {
      url "http://web03/stats"
      interval 10s
      label_set server_id "web03"
    }



### Run multiple metricd instances

The other method of consolidating metrics from multiple machines is to run a
metricd instance on each of the machines plus another central metricd instance
that pulls from the other metricd servers using federation. This is especially
useful if your metrics are not currently exposed via HTTP or SNMP and you have
to collect them locally on each machine.

Reusing our example from above, we would start this config on _each_ of the three
webservers:

    backend "sqlite:///var/tmp/metrics-local.sqlite"

    label_set server_id "${hostname}"

    metric number_of_requests_processed {
      kind monotonic(uint64)
      labels server_id
      rate 1s
    }

    collect_http {
      url "http://localhost/stats"
      interval 10s
    }

Then we would run a fourth metricd instance that pulls the data from the three
other instances:

    backend "sqlite:///var/tmp/metrics-global.sqlite"

    metric number_of_requests_processed {
      kind monotonic(uint64)
      labels server_id
      rate 1s
    }

    collect_metricd {
      addr web01:8175
    }

    collect_metricd {
      addr web02:8175
    }

    collect_metricd {
      addr web03:8175
    }

