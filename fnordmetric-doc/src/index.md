FnordMetric v1.1.2 Documentation
--------------------------------

FnordMetric is a framework for collecting and displaying timseries data.
It is split up into three components:


### FnordMetric UI

FnordMetric UI is a HTML5 / JavaScript API that lets you plugin realtime data and timeseries
graphs into any webpage without having to write a single line of code. It gives you maximum
flexiblitiy as you can control the complete layout and style with HTML and CSS. FnordMetric UI
uses WebSockets to communicate with the backend. There are two backend implementations:
FnordMetric Classic (ruby + redis) or FnordMetric Enterprise (JVM). You can use FnordMetric UI
as a white label solution to power your custom realtime analytics apps.

<a href="/documentation/ui_index">Getting Started with FnordMetric UI &rarr;</a>


### FnordMetric Classic

FnordMetric Classic offers a ruby DSL for processing data streams and manipulating gauges that
get stored in redis. It includes a collection of pre-made UI widgets which you can use to build
beautiful web dashboards within minutes. You can also use the FnordMetric UI HTML5 API to display
the data on another website or build highly customized / white label views.

<a href="/documentation/classic_index">Getting Started with FnordMetric Classic &rarr;</a>


### FnordMetric Enterprise

FnordMetric Enterprise is a JVM based timeseries database which serves as a backend for FnordMetric
UI. It can handle thousands of gauges and years worth of historical data. It also includes some
advanced features like CSV Export.

<i style="color:#999;">FnordMetric Enterprise is currently in private beta</i>
