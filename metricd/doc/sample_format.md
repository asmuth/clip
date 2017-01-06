Sample Format
=============

This page describes the various formats in which metricd can read and output
samples:


Text Format
-----------

Each packet must contain one or more lines. Each line is expected to be in
this format (series_id is optional):

    <metric_id>:<value>
    <metric_id>[<series_id>]:value

For example, this would insert the value `1.54` into the metric `load_average`.

    load_average:1.54

To specify a series id, include it in square brackets after the metric name:

    load_average[hostxy.prod.mycorp.com]:1.54

You can include more than one line in a packet. The lines may be separated with
`\n` or `\r\n`. To insert the values 23, 42 and 5 into the metrics
`mymetric-one`, `mymetric-two` and `mymetric-three` respectively, you would send
this packet:

    mymetric-one:23\r\n
    mymetric-two:42\r\n
    mymetric-three:5\r\n



JSON Format
-----------
