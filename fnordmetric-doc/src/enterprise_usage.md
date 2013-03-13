Configuration
-------------

FnordMetric Enterprise is configures using command line options. The valid options are:

<table>
  <tr>
    <td>--tcp <i>port</i></th>
    <td>
      Start the TCP server on this port.
    </td>
  </tr>
  <tr>
    <td>--tcp-threads <i>num_threads</i></th>
    <td>
      Number of worker threads for the TCP server.
    </td>
  </tr>
  <tr>
    <td>--udp <i>port</i></th>
    <td>
      Start the UDP server on this port.
    </td>
  </tr>
  <tr>
    <td>--udp-threads <i>num_threads</i></th>
    <td>
      Number of worker threads for the UDP server.
    </td>
  </tr>
  <tr>
    <td>--websocket <i>port</i></th>
    <td>
      Start the WebSocket server on this port.
    </td>
  </tr>
  <tr>
    <td>--websocket-threads <i>num_threads</i></th>
    <td>
      Number of worker threads for the WebSocket server.
    </td>
  </tr>
  <tr>
    <td>--http <i>port</i></th>
    <td>
      Start the HTTP server on this port.
    </td>
  </tr>
  <tr>
    <td>--http-threads <i>num_threads</i></th>
    <td>
      Number of worker threads for the HTTP server.
    </td>
  </tr>
  <tr>
    <td>--admin <i>port</i></th>
    <td>
      Start the Admin interface on this port.
    </td>
  </tr>
  <tr>
    <td>-d</th>
    <td>
      Run in debug mode.
    </td>
  </tr>
</table>
<br />

**Example**:

    java -jar FnordMetric-Enterprise-v1.2.7.jar --tcp 8922 --websocket 8080 --admin 8081

This will start FnordMetric, listen on TCP port 8922, start a WebSocket server on port
8080 and start the admin interface on port 8081.
