Metric Configuration
=============

Here is a simple example for a metric configuration:

    metric disk_space_used {
      type sample(uint64)
      granularity 1m
      unit byte
    }


This is the list of valid stanzas within the `metric` block:

<table>
  <thead>
    <tr>
      <th>Stanza</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>kind</strong></code></td>
      <td>Set the metric's type. See <a href="/documentation/metric-types">Metric Types</a></td>
    </tr>
    <tr>
      <td><code><strong>unit</strong></code></td>
      <td>Set the metric's unit. See <a href="/documentation/units">Units of Measurement</a></td>
    </tr>
    <tr>
      <td><code><strong>granularity</strong></code></td>
      <td>Set the storage granularity</td>
    </tr>
    <tr>
      <td><code><strong>rate</strong></code></td>
      <td>Optionally enable rate calculation for this metric</td>
    </tr>
    <tr>
      <td><code><strong>label</strong></code></td>
      <td></td>
    </tr>
    <tr>
      <td><code><strong>label_set</strong></code></td>
      <td></td>
    </tr>
   </tbody>
</table>


Stanzas
-------

kind <kind>

  Set the kind of a metric (mandatory)

  Examples:
    - sample(uint64)
    - counter(uint64)
    - monotonic(uint64)
    - min(uint64) / max(uint64)
    - average(uint64)
    - distribution
    - uniqueset
    - string


--------------------------------------------------------------------------------

unit <unit>

  Set the unit of a metric

  Examples.
    - rate(bytes, "1h")


--------------------------------------------------------------------------------

granularity <granularity>

  Controls the granularity with which samples are recorded. A granularity of
  "1min" will lead to one aggregated value being written to disk every 1 second.

  Examples:
    - 3600
    - 1s
    - 5m
    - 12h
    - 1w


--------------------------------------------------------------------------------

retention <retention>

  Controls how long samples for this metric are stored (the default is unlimited,
  which means samples are never discarded)

  Examples:
    - 5m
    - 12h
    - 1w
    - 1y
    - unlimited (default)



--------------------------------------------------------------------------------

summarize <fn>

  Set the default summarization method (i.e. the method by which multiple series
  are combined into a "total")

  Examples:
    - sum
    - min/max
    - avg
    - median


--------------------------------------------------------------------------------

smoothing <fn>

  Sets a smoothing function (the smoothing function is applied at display time)

  Examples:
    - moving_avg("20s")


--------------------------------------------------------------------------------

interpolate <on|off>

  Turn interpolation {on,off}


--------------------------------------------------------------------------------

display_granularity <granularity>

  Controls the granularity with which samples are displayed. If the
  display_granularity is larger than the storage granularity, downsampling will
  occur (this always gives correct results). However, if the display granularity
  is smaller than the storage granularity, upsampling/extrapolation must be
  performed to produce a continous timeseries. This behaviour can be disabled
  by specifying "interpolate off".

  Examples:
    - 3600
    - 1s
    - 5m
    - 12h
    - 1w


--------------------------------------------------------------------------------

chart_type <line|area|bars>

   ...


--------------------------------------------------------------------------------

log_scale <on|off>

  Turn logarithmic scales {on,off} for this metric



