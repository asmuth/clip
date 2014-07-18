FnordMetric UI Examples
-----------------------

If you want to add examples, blogs or clients, please send a pull
request for this page on github.


### Examples

_If you want to add examples, blogs or clients, please send a pull request for this page on github._


<!-- here be dragons: how to implement custom date / time controls -->


### Customizing the widgets

Since all widgets are rendered with HTML5 + SVG (d3.js) you can control
the style with CSS.

Example: This five-line CSS snippet applies a "dark" style:

    body{ background:#111; }
    body .fnordmetric_legend li.line .label{ color:#fff; }
    body .fnordmetric_graph .y_grid .tick { stroke:rgba(255,255,255,.15); stroke-dasharray:0; }
    body .fnordmetric_graph .y_ticks text { fill:#ffffff; }
    body .fnordmetric_graph .x_tick .title { color:#ffffff; }
