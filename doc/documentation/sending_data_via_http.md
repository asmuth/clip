Sending data via HTTP
=====================

You can send metric data to FnordMetric Server using a simple HTTP interface.

---

To use the http interface, start FnordMetric Server with the --http_port
option:

    $ fnordmetric-server --storage_backend inmemory --http_port 8080

The fnordmetric server will listen for HTTP connections on the specified port.
To insert a sample, send a `POST /metrics` request. There are two mandatory 
parameters `metric` and `value`: The metric parameter must contain the metric
name to insert into and the value parameter must contain the floating point
value to insert. [Check out the full HTTP API here](/documentation/http_api)

    POST /metrics
    metric=<metric-name>&value=<value>

The simplest way to send in metrics via HTTP from your command line if you have
FnordMetric Server running on HTTP port 8080 would be using the curl utility:

    $ curl -X POST -d "metric=test_metric&value=23.5" localhost:8080/metrics

This would insert the value "23.5" into the metric "test_metric". If no metric
with this key exists yet, a new one will be created.


Adding Metric Labels
--------------------

To allow you to drill down into your metric data in arbitrary dimensions, each
sample can optionally be labelled with one or more "labels". Each label is a
key: value pair and is specified as an additional HTTP POST parameter where the
key follows this format: `label[...]`

    POST /metrics
    metric=<metric-name>&value=<value>&label[<k1>]=<v1>

    POST /metrics
    metric=<metric-name>&value=<value>&label[<k1>]=<v1>&label[<k2>]=<v2>

For example, if we want to sample the CPU utilization on multiple hosts into
the metric "cpu-utilization" with two label dimensions "hostname" and "datacenter",
our HTTP requests should look like this:

_Insert value `0.642` with labels `hostname=machine83` and `datacenter=ams1` into metric `cpu-utilization`:_

    >> POST /metrics?metric=cpu-utilization&value=0.642&label[hostname]=machine83&label[datacenter]=ams1
    << HTTP/1.1 201 CREATED


#### Curl and square brackets

Alas, curl interprets square brackets as globs, so we have to escape the square
brackets when using the curl command line utility:


    $ curl -X POST -d "metric=cpu-util&label\[hostname\]=machine83&label\[datacenter\]=ams1&value=0.642" localhost:8080/metrics


#### Examples

    >> POST /metrics?metric=total_sales_in_euro-sum-30&value=351 HTTP/1.1
    << HTTP/1.1 201 CREATED

    >> POST /metrics?metric=http_status_codes&value=351&label[statuscode]=200&label[hostname]=myhost1 HTTP/1.1
    << HTTP/1.1 201 CREATED


