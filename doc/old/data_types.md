Data Types
==========

Since metric-collectd supports a number of pluggable backend databases, it needs
map all column data types from an internal set of types to the native types of
the backend database.

This is the list of valid data types. Please check the backend-specific
documentation for more detailed information to which types they map in the backend:

<table>
  <thead>
    <tr>
      <th>Type</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>string</strong></code></td>
      <td>Stores a UTF-8 string</td>
    </tr>
    <tr>
      <td><code><strong>uint64</strong></code></td>
      <td>Stores a 64-bit unsigned integer</td>
    </tr>
    <tr>
      <td><code><strong>int64</strong></code></td>
      <td>Stores a 64-bit signed integer</td>
    </tr>
    <tr>
      <td><code><strong>float64</strong></code></td>
      <td>Stores a 64-bit floating point value.</td>
    </tr>
  </tbody>
</table>
