Units
=====

Units define how a numeric value is displayed. For example, a unit configuration
could tell metricd to format the number "1,024" as "1KiB" (unit=bytes_binary) or
as "1.24KV" (unit=volt).


Unit Configuration
------------------

To configure a unit, add a `unit` block to your configuration file:

    unit my_fancy_unit {
      // unit configuration here
    }


Rates
-----

Built-in Units
--------------

All of the built-in units are stored in the default configuration file
`/etc/metricd/units_default.conf`. So you can easily$ change or remove the
built-in units.


<table>
  <thead>
    <tr>
      <th>Unit</th>
      <th>Unit Names</th>
      <th>Base Value</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>bytes</strong></code></td>
      <td><code>kilobytes</code>, <code>kilobytes</code>, <code>kilobytes</code>, <code>kilobytes</code></td>
      <td>1 byte</td>
    </tr>
   </tbody>
</table>
