Summarization
=============

The metricd server performs two types of summarization on metrics: group
summarization and gross summarization:

- **Group summarization** controls how multiple series from the
same metric are combined into an "aggregate" timeseries.

- **Gross summarization** is used to roll up a timeseries into a single scalar value.
This could e.g. be the sum or the average of all points in the timeseries.


Gross Summary Functions
-----------------------

<table>
  <thead>
    <tr>
      <th>Function</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>sum</strong></code></td>
      <td>Return the sum of all points in the timeseries</td>
    </tr>
    <tr>
      <td><code><strong>avg</strong></code></td>
      <td>Return the average of all points in the timeseries</td>
    </tr>
    <tr>
      <td><code><strong>mean</strong></code></td>
      <td>Return the mean of all points in the timeseries</td>
    </tr>
    <tr>
      <td><code><strong>stddev</strong></code></td>
      <td>Return the standard deviation of all points in the timeseries</td>
    </tr>
    <tr>
      <td><code><strong>min</strong></code></td>
      <td>Return the smallest from all points in the timeseries</td>
    </tr>
    <tr>
      <td><code><strong>max</strong></code></td>
      <td>Return the largest from all points in the timeseries</td>
    </tr>
    <tr>
      <td><code><strong>trend</strong></code></td>
      <td>Return the relative change in the timeseries</td>
    </tr>
   </tbody>
</table>


Group Summary Functions
-----------------------

<table>
  <thead>
    <tr>
      <th>Function</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>sum</strong></code></td>
      <td>Return the sum of all series values</td>
    </tr>
    <tr>
      <td><code><strong>avg</strong></code></td>
      <td>Return the average of all series values</td>
    </tr>
    <tr>
      <td><code><strong>mean</strong></code></td>
      <td>Return the mean of all series values</td>
    </tr>
    <tr>
      <td><code><strong>stddev</strong></code></td>
      <td>Return the standard deviation of all series values</td>
    </tr>
    <tr>
      <td><code><strong>min</strong></code></td>
      <td>Return the smallest from all series values</td>
    </tr>
    <tr>
      <td><code><strong>max</strong></code></td>
      <td>Return the largest from all series values</td>
    </tr>
   </tbody>
</table>
