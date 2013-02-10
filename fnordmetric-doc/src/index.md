<h2>FnordMetric v1.1.2 Documentation</h2>
<p>FnordMetric is a framework for collecting and displaying timseries data. It is split up into
  three components:</p>

<h3>FnordMetric UI</h3>
<p>FnordMetric UI is a HTML5 / JavaScript API that lets you plugin realtime data and timeseries
  graphs into any webpage without having to write a single line of code. It gives you maximum
  flexiblitiy as you can control the complete layout and style with HTML and CSS. FnordMetric UI
  uses WebSockets to communicate with the backend. There are two backend implementations:
  FnordMetric Classic (ruby + redis) or FnordMetric Enterprise (JVM). You can use FnordMetric UI
  as a white label solution to power your custom realtime analytics apps.</p>
  <a href="#">Getting Started with FnordMetric UI &rarr;</a>

<h3>FnordMetric Classic</h3>
<p>FnordMetric Classic offers a ruby DSL for processing data streams and manipulating gauges that
  get stored in redis. It includes a collection of pre-made UI widgets which you can use to build
  beautiful web dashboards within minutes. You can also use the FnordMetric UI HTML5 API to display
  the data on another website or build highly customized / white label views.</p>
  <a href="#">Getting Started with FnordMetric Classic &rarr;</a>

<h3>FnordMetric Enterprise</h3>
<p>FnordMetric Enterprise is a JVM based timeseries database which serves as a backend for FnordMetric
  UI. It does not offer it's own user interface.</p>
  <a href="#">Getting Started with FnordMetric Enterprise &rarr;</a>

