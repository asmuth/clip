Configuration File
==================

This is a list of all supported top-level stanzas in the `metric-collectd`
configuration file. For more information on the semantics of the statements,
please see the [Data Model](/documentation/data-model) page.

<table>
  <thead>
    <tr>
      <th>Setting</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>backend</strong></code></td>
      <td>Connection string/URL for the backend database (e.g. `mysql://localhost:3306/mydb?user=root`)</td>
    </tr>
    <tr>
      <td><code><strong>table</strong></code></td>
      <td>Declares a target table, i.e. a table into which measurements should be stored (A data sink).</td>
    </tr>
    <tr>
      <td><code><strong>create_tables</strong></code></td>
      <td>This settings controls wether tables should be automatically created (if they don't exist yet). Valid values are `on` and `off`</td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-statsd">listen_udp</a></strong></code></td>
      <td>Declares a UDP/statsd listener port (A data source).</td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-http">listen_http</a></strong></code></td>
      <td>Declares a HTTP listener port (A data source).</td>
    </tr>
    <tr>
      <td><code><strong>fetch_snmp</strong></code></td>
      <td>Pull data via SNMP (A data source).</td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-http">fetch_http</a></strong></code></td>
      <td>Pull data via HTTP (A data source).</td>
    </tr>
  </tbody>
</table>


## Table Configuration

Each table is defined within a single `table` block:

    table <table_name> {
      # table config here
    }

This is the list of valid stanzas within the table block:

#### interval

The interval stanza controls the aggregation window for a table. There must not
be more than one interval stanza per table block.

    interval <value>

Valid values are any number (interpreted as seconds) or strings of the format
`N[s(((ec)ond)s)|m(((in)ute)s)|h((our)s)|d((ay)s)|w((eek)s)|y((ear)s)]`. For
example: `1s`, `30min` or `6hours`.

#### label

The label stanza adds a label column to the table. There may be any number of
label stanzas (including zero) in a table block.

    label <column_name> [<data_type>]

The second data type parameter is optional. If it is set the value must be
any of the valid data types below (e.g. `string` or `uint64`).

#### measure

The measure stanza adds a measurement column to the table. There may be any number
of measure stanzas in a table block.

    measure <column_name> <aggregation_fun>

The format of the aggregation function parameter is in the format `function(type)`,
for example `sum(uint64)` or `average(float64)` or `count_distinct(string)`
