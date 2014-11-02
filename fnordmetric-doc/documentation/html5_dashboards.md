FnordMetric UI
--------------

<i>This tutorial assumes that you have either FnordMetric Classic or FnordMetric
Enterprise running as a backend</i>

The FnordMetric UI HTML5 API allows you to plugin real-time data and
charts into any website without having to write any code. This is achieved
by including a JavaScript library and using data-* attributes on html
elements to declare the widgets.

FnordMetric UI requires jQuery 1.6.2+. To set up the basic HTML structure save
this to a file `my_dashboard.html`. Download the two files `fnordmetric-ui.js`
and `fnordmetric-ui.css` [from here](/documentation/downloads) and put them into
the same folder.

    <!DOCTYPE html>
    <html>
      <head>
        <title>FnordMetric</title>
        <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.6.2/jquery.min.js"></script>
        <link href='fnordmetric-ui.css' type='text/css' rel='stylesheet' />
        <script src='fnordmetric-ui.js' type='text/javascript'></script>
      </head>
      <body>
        ...
      </body>
    </html>


We will display one counter "total sales in the last hour" on our page. To do that we
first have to connect to the FnordMetric Backend using WebSockets. This examples assumes
that you have either FnordMetric Classic running on port 4242 or FnordMetric Enterprise
with a WebSocket listener on port 4242.

    <script>
      FnordMetric.setup({
        "address":   "localhost:4242",
        "namespace": "myapp"
      });
    </script>


_Note: If you are using FnordMetric Classic, make sure you set the correct namespace (it is
"myapp" below). If you are using FnordMetric Enterprise you don't have to specify a
namespace at all)_:

Now the FnordMetric UI library is loaded and we can plug widgets into the page
using HTML5 elements. Let's start with a simple counter that displays the sum of
sales in the last hour and updates itself every second:

    Total Sales in the last hour:
    <span
      data-fnordmetric="counter"
      data-at="sum(-1hour)"
      data-gauge="total_sales-sum-10"
      data-autoupdate="1"
      data-unit="$"
      >0</span>


If you open `my_dashboard.html` in your browser, You should see a
page displaying "0". It's a good idea to open the JavaScript / Inspector
console of your browser as FnordMetric UI will print error messages using `console.log`

The last step is to start sending data. We will report one sale of 29$.

If you are using FnordMetric Enterprise you can do this with e.g. netcat:

    echo "SAMPLE total_sales-sum-10 29" | nc localhost 8922

If you are using FnordMetric Classic you can do this e.g. with curl + http:

    curl -X POST -d '{ "_type": "_incr", "value": 29, "gauge": "total_sales-sum-10" }' http://localhost:4242/events


For fun and profit, we can display a timeseries graph of sales in the last 10 minutes with this snippet:

    <div
      data-fnordmetric="timeseries"
      data-since="-10minutes"
      data-until="now"
      data-gauges="total_sales-sum-10"
      data-autoupdate="1"
      ></div>


Now your dashboard should look like this:

<img src="/img/fnordmetric_ui_example_screen.png" width="630" class="shadow" />
<br />

#### But wait, my dashboard is white?

Yes, I skipped this part to make the into a bit shorter. The example above
uses a "custom" styesheet. Since all widgets are rendered with HTML5 + SVG (d3.js) you
can control the style with CSS.

This five-line CSS snippet is all it takes to apply the "dark" style from above:

    body{ background:#111; }
    body .fnordmetric_legend li.line .label{ color:#fff; }
    body .fnordmetric_graph .y_grid .tick { stroke:rgba(255,255,255,.15); stroke-dasharray:0; }
    body .fnordmetric_graph .y_ticks text { fill:#ffffff; }
    body .fnordmetric_graph .x_tick .title { color:#ffffff; }


Have a look at the full [API Reference](/documentation/ui_html5_api) to see what else is possible.
