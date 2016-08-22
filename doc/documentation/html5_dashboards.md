Getting Started with HTML5 Dashboards
=====================================

<i>This guide will walk you through building a simple dashboard using ChartSQL
and HTML5. You need a running FnordMetric Server instance. If you do not have
FnordMetric running yet, read the [Getting started with Fnordmetric Server page](documentation/getting_started/fnordmetric-server)
first.</i>


The FnordMetric HTML5 API allows you to plug query results and charts into any
website without having to write any code. This is achieved by including a small
JavaScript library and using the component html tags: `\<fm-chart\>`, `\<fm-table\>`.

#### Getting Started

For our example, we will build a dashboard that displays the total sales amount of
a fictional web shop. Let's start by creating a metric called "myshop_sales" that
records the sales amount in cents for each sale. To create the metric and insert
the first sample using the HTTP API, run this from your command line:

    $ curl -X POST -d "metric=myshop_sales&value=2350" localhost:8080/metrics

In our example, this would record a single sale of 23.50 in or shop. Execute the
command a few times with different values to simulate a few "sales". Then open
the FnordMetric Server web UI on http://localhost:8080/ in your browser and
execute this query to show the data you just inserted:

    SELECT time, value FROM myshop_sales;

To make a chart from the values, execute this query (note that you need to insert
at least two values to get a reasonable output):

    DRAW LINECHART
        AXIS BOTTOM;

    SELECT time AS x, value AS y
        FROM myshop_sales;


#### Embedding Charts in HTML5

The next step is to embed the query we just wrote into a HTML page to create a
simple dashboard. To set up the basic HTML structure save this to a new file
and call it `my\_dashboard.html`:

    <!DOCTYPE html>
    <html>
      <head>
        <title>My Dashboard</title>
        <script src='http://localhost:8080/s/fnordmetric.js' type='text/javascript'></script>
      </head>
      <body>
        ...
      </body>
    </html>

**Note that the example above assumes you have FnordMetric Server running on
`localhost:8080`. Change if appropriate**

---

The HTML5 API offers a number of components that you can use without writing any
JavaScript. To display our chart, we will use the `\<fm-chart\>` component. Put
this in the body of the `my_dashboard.html` file we created earlier.

    <fm-chart style="width: 800px;">
        DRAW LINECHART
            AXIS BOTTOM;

        SELECT time AS x, value AS y
            FROM myshop_sales;
    </fm-chart>


If you now open `my_dashboard.html` in your browser, you should see a
page displaying a simple line chart. 

<img src="/img/fnordmetric_ui_example_screen.png" width="800" style="margin: 20px 0 40px 0;" class="shadow" />
<br />

Have a look at the full [API Reference](/documentation/html5_api_reference/) to see what else is possible.
