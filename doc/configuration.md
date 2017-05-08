Configuration File
==================

This is a list of all supported top-level stanzas in the configuration file.
For more information on the semantics of the statements, please refer to the
linked pages.

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
      <td><code><strong>metric</strong></code></td>
      <td>Declares a target table, i.e. a table into which measurements should be stored (A data sink).</td>
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
      <td><code><strong><a href="/documentation/collect-data-via-http">fetch_http</a></strong></code></td>
      <td>Pull data via HTTP (A data source).</td>
    </tr>
    <tr>
      <td><code><strong><a href="/documentation/collect-data-via-shell">fetch_proc</a></strong></code></td>
      <td>Pull data via HTTP (A data source).</td>
    </tr>
    <tr>
      <td><code><strong>labels</strong></code></td>
      <td></td>
    </tr>
    <tr>
      <td><code><strong>label_set</strong></code></td>
      <td></td>
    </tr>
  </tbody>
</table>


### Configuration File Locations

