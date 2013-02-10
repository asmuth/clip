
 + refresh
 + resize
 + customize style -> css (ex. dark)


Timeseries
----------

  data-fnordmetric
    must be "timeseries" (mandatory)

  data-gauges
    comma seperated list of gauges to be dispalyed (mandatory)

  data-since, data-until
    these attributes specify the time range to be displayed. they may be a
    unix timestamp, a time definition like "-3hours" (3 hours ago), "-45m"
    (45m ago) or "-3600" (3600 seconds = 1 hour ago) or "now". To e.g. display
    the last two hours of data use: data-since="-2hours" data-until="now"

  data-colors
    comma seperated list of series / gauge colors (optional)

  data-height
    height of the chart (default is 240px)

  data-chart-style
    render style (either line, area or flow), default is "line"

  data-cardinal
    use cardinal splines (values are "on" or "off"), default is "off"

  data-autoupdate
    refresh this chart every N seconds, default is no autorefresh



Counters
--------

  data-fnordmetric
    must be "counter"

  data-gauge
    name of the gauge to be displayed

  data-at
    display this value at a certain time, may be a unix timestamp,
    a time definition like "-3hours" (3 hours ago), "-45m" (45m ago)
    or "-3600" (3600 seconds = 1 hour ago).
    You can also get a sum of the last x measurements by setting this to
    e.g. "sum(-3hours)" which will give you the sum of the last three hours.
    You can also get a average of the last x measurements by setting this to
    e.g. "avg(-3hours)" which will give you the average of the last three hours.
    The Default is "now".

  data-autoupdate
    refresh this value every N seconds

  data-unit
    display the value with a unit (e.g. € or ms)

