Sending data via statsd
=======================

FnordMetric Server allows you to send metric data using the statsd protocol.
Statsd is a very simple, text based protocol.

---

To use the statsd protocol, start the fnordmetric server with the --statsd_port
option:

    $ fnordmetric-server --storage_backend inmemory --statsd_port 8125 --http_port 8080

The fnordmetric server will listen for UDP packets on the specified port. Each
UDP packet must contain one or more ASCII lines. Each line is expected to be in
this format:

    <metricname>:<value>

So the simplest way to send in metrics from your command line if you have 
fnordmetric server running on port 8125 would be:

    $ echo "foo:1" | nc -u -w0 127.0.0.1 8125

This would insert the value "1" into the metric "foo". If no metric with this key
exists yet, a new one will be created.


Using metric labels with statsd
-------------------------------

The statsd protocol itself does not support labels. However, the statsd
implementation in FnordMetric Server respects a metric naming convention
that allows you to specify labels.

The line format for metrics with one label is:

    <metricname>[<label_key>=<label_value>]:<value>

To attach multiple labels, repeat the square bracket clause:

    <metricname>[<label1_key>=<label1_value>][<label2_key>=<label2_value>]:<value>

For example, if we want to sample the CPU utilization on multiple hosts into
the metric "cpu-utilization" with two label dimensions "hostname" and "datacenter",
our UDP packets could look like this:

_Insert value `0.642` with labels `hostname=machine83` and `datacenter=ams1` into metric `cpu-utilization`:_

    cpu-utilization[hostname=machine83][datacenter=ams1]:0.642

Batch insert
------------

You may include multiple lines in the same UDP packet. The lines may be
separated with `\n` or `\r\n`.

For example, to insert the values 23, 42 and 5 into the metrics "mymetric-one",
"mymetric-two" and "mymetric-three", you would send this UDP packet:

    mymetric-one:23\r\n
    mymetric-two:42\r\n
    mymetric-three:5\r\n


