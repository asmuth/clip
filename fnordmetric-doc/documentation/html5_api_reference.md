FnordMetric HTML5 API Reference
===============================

_The HTML5 API is still a Work In Progress &mdash; Pull Requests welcome!_

### Introduction

The FnordMetric UI HTML5 API allows you to plugin real-time data and
charts into any website without having to write any JavaScript code. This is
achieved by including a JavaScript library and using the FnordMetric HTML5
components.

The JavaScript library `fnordmetric.js` is bundled with FnordMetric Server, to
include it put this line into your HTML file:

    <!-- include fnordmetric.js from fnordmetric-server-host:port -->
    <script src='http://fnordmetric-server-host:port/s/fnordmetric.js' type='text/javascript'></script>

    <!-- example: include fnordmetric.js from localhost:8080 -->
    <script src='http://localhost:8080/s/fnordmetric.js' type='text/javascript'></script>


### Component: \<fm-chart\>

The `\<fm-chart\>` tag executes a ChartSQL query and displays the result as a chart.
For Example:

    <fm-chart style="width: 800px;">
      DRAW LINECHART
          AXIS BOTTOM;

      SELECT
          time AS x, value AS y FROM ...;
    </fm-chart>

These are the valid html attributes for `\<fm-chart\>` tags:

<table>
  <tr>
    <th>data-autoupdate</th>
    <td>
      refresh the chart every N seconds
    </td>
  </tr>
</table>
