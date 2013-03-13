Configuration
-------------

FnordMetric Enterprise is configured using command line options.

**Example**:

    java -jar FnordMetric-Enterprise-v1.2.7.jar --tcp 8922 --websocket 8080 --admin 8081

This will start FnordMetric, listen on TCP port 8922, start a WebSocket server on port
8080 and start the admin interface on port 8081.

### Options

The valid options are:

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
      Number of worker threads for the TCP server. (default: 4)
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
      Number of worker threads for the UDP server. (default: 4)
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
      Number of worker threads for the WebSocket server. (default: 4)
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
      Number of worker threads for the HTTP server. (default: 4)
    </td>
  </tr>
  <tr>
    <td>--admin <i>port</i></th>
    <td>
      Start the Admin interface on this port.
    </td>
  </tr>
  <tr>
    <td>--swapdir <i>path</i></th>
    <td>
      Directory in which FnordMetric will store the metric peristence files on disk (default: /tmp)
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
