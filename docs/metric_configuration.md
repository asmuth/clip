Metric Configuration
====================

Here is a simple example for a metric configuration:

    metric disk_space_used {
      kind gauge(uint64)
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
      <td>Optionall add one or more label names to this metric</td>
    </tr>
    <tr>
      <td><code><strong>label_set</strong></code></td>
      <td>Set the (default) value for one of the label names</td>
    </tr>

   </tbody>
</table>

