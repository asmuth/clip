Metric Configuration
=============

Here is a simple example:

    metric disk_space_used {
      type sample(uint64)
      granularity 1m
      unit byte
    }


List of valid stanzas within the `metric` block:

<table>
  <thead>
    <tr>
      <th>Stanza</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>type</strong></code></td>
      <td>Set the metric's type</td>
    </tr>
    <tr>
      <td><code><strong>unit</strong></code></td>
      <td>Set the metric's unit</td>
    </tr>
    <tr>
      <td><code><strong>unit_scale</strong></code></td>
      <td></td>
    </tr>
    <tr>
      <td><code><strong>granularity</strong></code></td>
      <td>Set the storage granularity</td>
    </tr>
    <tr>
      <td><code><strong>retention</strong></code></td>
      <td>Set the rentention period</td>
    </tr>
    <tr>
      <td><code><strong>summarize_group</strong></code></td>
      <td></td>
    </tr>
    <tr>
      <td><code><strong>summarize_gross</strong></code></td>
      <td></td>
    </tr>
    <tr>
      <td><code><strong>interpolate</strong></code></td>
      <td></td>
    </tr>
    <tr>
      <td><code><strong>smoothing</strong></code></td>
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



