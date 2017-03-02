Aggregation Functions
=====================

Aggregation functions control how measurements are pre-aggregated before being
stored into the backend database. Please see the
[Table Configuration](/documentation/table-configuration) page for more details.

This is the list of all currently supported aggregation functions and a description
for each of the functions:

<table>
  <thead>
    <tr>
      <th>Name(s)</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>sum(T)</strong></code>, <code><strong>counter(T)</strong></code></td>
      <td>Returns the sum of all input values</td>
    </tr>
    <tr>
      <td><code><strong>max(T)</strong></code></td>
      <td>Returns the largest input value</td>
    </tr>
    <tr>
      <td><code><strong>min(T)</strong></code></td>
      <td>Returns the smallest input value</td>
    </tr>
  </tbody>
</table>

