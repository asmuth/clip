Sample Format
=============

This page describes the various formats in which metric-collectd accepts input
samples. The formats are generally the same for all pull and push transport
protocols (currently UDP and HTTP).


Text/StatsD Format
------------------

The `statsd` format is compatible with the StatsD and Prometheus/Borgmon
format. A packet must contain one or more lines. Every line is expected to
conform to this scheme (labels are optional):

    metric_name:value
    metric_name{label1=value1,label2=value2,...}:value

For example, this would insert the value `1.54` into the metric `host_stats.load_avg`.

    host_stats.load_avg:1.54

To specify labels, include them in curly braces after the metric name. Multiple
labels are separated by comma.

    host_stats.load_avg{hostname="hostxy.prod.mycorp.com"}:1.54

You can include more than one line in a packet. The lines may be separated with
`\n` or `\r\n`. To insert the values 23, 42 and 5 into the metrics
`mymetric-one`, `mymetric-two` and `mymetric-three` respectively, you would send
this packet:

    mymetric-one:23\r\n
    mymetric-two:42\r\n
    mymetric-three:5\r\n

The colon may be replaced by whitespace for compatibility with other systems:

    metric_name value
    metric_name{label1=value1,label2=value2,...} value

For example:

    host_stats.load_avg 1.54
    host_stats.load_avg{hostname="hostxy.prod.mycorp.com"} 1.54

