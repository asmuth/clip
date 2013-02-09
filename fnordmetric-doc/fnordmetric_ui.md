


Counters
--------

  data-fnordmetric
    must be "counter"

  data-gauge
    name of the gauge to be displayed

  data-at
    display this value at a certain time, may be a unix timestamp,
    a time definition like "-3hours" (3 hours ago), "-45m" (45m ago)
    or "-3600" (3600 seconds = 1 hour ago). You can also get a sum
    by setting this to e.g. "sum(-3hours)" which will give you the
    sum of the last thee hours. The Default is "now".

  data-autoupdate
    refresh this value every N seconds

  data-unit
    display the value with a unit (e.g. € or ms)

