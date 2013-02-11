FnordMetric UI
--------------

<i>This tutorial assumes that you have either FnordMetric Class or FnordMetric
Enterprise running as a backend</i>


#### But wait, my dashboard is white?

Yes, I skipped this part to make the getting started a bit shorter. The example above
uses a "custom" styesheet. Since all widgets are rendered with HTML5 + SVG (d3.js) you
can control the style with CSS.

This five-line CSS snippet is all it takes to apply the "dark" style from above:

    body{ background:#111; }
    body .fnordmetric_legend li.line .label{ color:#fff; }
    body .fnordmetric_graph .y_grid .tick { stroke:rgba(255,255,255,.15); stroke-dasharray:0; }
    body .fnordmetric_graph .y_ticks text { fill:#ffffff; }
    body .fnordmetric_graph .x_tick .title { color:#ffffff; }
