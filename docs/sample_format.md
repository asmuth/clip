Input Formats
=============

This page describes the various formats in which the metrictools accepts input
measurements. The formats are generally the same for all pull and push transport
protocols.

These are the supported measurement encodings:

<table>
  <thead>
    <tr>
      <th style="width: 26%;">Type</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>statsd</strong></code></td>
      <td>The statsd format</td>
    </tr>
    <tr>
      <td>
        <code><strong>borgmon</strong></code>,
        <code><strong>prometheus</strong></code>
      </td>
      <td>The borgmon/prometheus format</td>
    </tr>
  </tbody>
</table>

StatsD Format
--------------

A packet must contain one or more lines. Every line is expected to conform to
this scheme (labels are optional):

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


Borgmon/Prometheus Format
-------------------------

Please refer to the [https://prometheus.io/docs/instrumenting/exposition_formats/](https://prometheus.io/docs/instrumenting/exposition_formats/) page.
