Gauges
------

The basic concept in FnordMetric is called a "gauge". A Gauge is a primitive data store for a numerical value over
time. A gauge has two dimensions: Value und Time. Gauges can be used in different modes like sum,
average, max/min, etcetara.


### Create gauges on the fly

The easiest way to submit data to FnordMetric is to use zero-config gauges. These
are very similar to counters in Statsd: you don't have to create the gauges
upfront, they are automatically created as you send events. These events have
to follow a strict schema:
<hr class="space" />

Example: _Increment the gauge `sales-per-second` by 4, gauge resolution is 1 second._

    { "_type": "_incr", "value": 4, "gauge": "sales-per-second", "flush_interval": 1 }

Example: _Sample average request time (this sample: 42ms), gauge resolution is 10 seconds._

    { "_type": "_avg", "value": 42, "gauge": "avg-request-time", "flush_interval": 10 }


Valid keys for these events are:

<table>
  <tr>
    <th><b>_type</b> <i>(mandatory)</i></th>
    <td>
      the operation to perform

      <p>allowed values are `_incr`, `_decr` and `_set` to
      increment, decrement and set counters and `_avg`, `_min`, `_max` to sample
      average, minimum or maximum values.</p>
    </td>
  </tr>
  <tr>
    <th><b>gauge</b> <i>(mandatory)</i></th>
    <td>
      name of the gauge. if it does not exist it will be created.
    </td>
  </tr>
  <tr>
    <th><b>flush_interval</b> <i>(mandatory)</i></th>
    <td>
      interval in which the value of this gauge is persisted. this is basically the
      gauge's resolution.
    </td>
  </tr>
</table>
<br />

### Create gauges by hand

FnordMetric also offers an api to create these gauges by hand.
