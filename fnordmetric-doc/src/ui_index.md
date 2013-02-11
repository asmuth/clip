FnordMetric UI
--------------

<i>This tutorial assumes that you have either FnordMetric Classic or FnordMetric
Enterprise running as a backend</i>

The FnordMetric UI HTML5 API allows you to plugin real-time data and
charts into any website without having to write code. This is achieved
by including a JavaScript library and using data-* attributes on html
elements to declare the widgets.

The javascript library `fnordmetric-ui.js` is bundled with FnordMetric
Classic and FnordMetric Enterprise, but you can download a copy here (FIXPAUL)
if you want to include it in your project.

FnordMetric UI requires jQuery 1.6.2+. We set up the basic HTML structure (this
assumes you have either FnordMetric Classic or FnordMetric Enterprise running
on port 4242) and save this to a fil `my_dashboard.html`

    <!DOCTYPE html>
    <html>
      <head>
        <title>FnordMetric</title>
        <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.6.2/jquery.min.js"></script>
        <link href='http://localhost:4242/fnordmetric-ui.css' type='text/css' rel='stylesheet' />
        <script src='http://localhost:4242/fnordmetric-ui.js' type='text/javascript'></script>
      </head>
      <body>
        ...
      </body>
    </html>


We will display on counter "total revenue in the last hour" on our page. First we
have to connect to the FnordMetric Backend using WebSockets:

    <script>
      FnordMetric.setup({
        "address":   "localhost:4242",
        "namespace": "myapp"
      });
    </script>

_Note: When using FnordMetric Enterprise the namespace is always "fnordmetric"_

Now the FnordMetric UI library is loaded and we can plug widgets into the page
using HTML5 elements. Let's start with a simple counter that display the sum of
sales in the last hour. And update every second:

    Total Sales in the last hour:
    <span
      data-fnordmetric="counter"
      data-at="sum(-1hour)"
      data-gauge="total_sales_in_euro"
      data-autoupdate="1"
      data-unit="€"
      >0</span>


Now open my_dashboard.html in the browser of your choice. You should see a
page displaying "0". It's a good idea to open the JavaScript / Inspector
console of your browser as FnordMetric UI will print error messages using `console.log`

The last step is to start sending data. This will report one sale of 29.99€:

    curl -X POST -d '{ "_type": "_incr", "value": 29.99, "gauge": "total_sales_per_minute" }' http://localhost:4242/events


For fun and profit, we can display a timeseries graph of sales in the last 10 minutes with this snippet:

    <div
      data-fnordmetric="timeseries"
      data-since="-10minutes"
      data-until="now"
      data-gauges="total_sales_in_euro"
      data-autoupdate="1"
      ></div>


Now your dashboard should look like this:

    FIXPAUL: screenshot


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


Have a look at the full [API Reference](/documentation/ui_html5_api) to see what else is possible.
