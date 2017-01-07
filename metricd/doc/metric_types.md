Metric Types
=============

<table>
  <thead>
    <tr>
      <th>Abstract Type</th>
      <th>Types</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>counter(T)</strong></code></td>
      <td><code>counter(uint64)</code>, <code>counter(int64)</code>, <code>counter(float64)</code></td>
      <td>Report cumulative counter increments</td>
    </tr>
    <tr>
      <td><code><strong>sample(T)</strong></code></td>
      <td><code>sample(uint64)</code>, <code>sample(int64)</code>, <code>sample(float64)</code></td>
      <td>Report point-in-time measurements</td>
    </tr>
    <tr>
      <td><code><strong>monotonic(T)</strong></code></td>
      <td><code>monotonic(uint64)</code>, <code>monotonic(int64)</code>, <code>monotonic(float64)</code></td>
      <td>Report a monotonically increasing number</td>
    </tr>
    <tr>
      <td><code><strong>healthcheck</strong></code></td>
      <td><code>healthcheck</code></td>
      <td>Report a healthy/unhealthy value</td>
    </tr>
   </tbody>
</table>


counter
-------

sample
------

monotonic
---------
