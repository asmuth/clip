Getting Started with fnordmetric-cli
====================================

This chapter is for impatient people who don't like reading documentation. For
more in-depth information you are kindly referred to the following chapters.

As the first simple example we will create a bar chart that displays gross
domestic product per country and looks like this:

     #
     #       #
     #       #      #
     #       #      #
     USA     UK     IRAN ...
    -----------------------------------------

Let's say we have a csv file containing the data that looks like this:

    country,gdp
    iran,8000
    uk,9000
    usa,1000

We can create the chart pictured above with this sql query"

    -- import the csv file as a table
    IMPORT gdp_per_country FROM "gdp_per_country.csv";

    -- draw a simple bar chart
    BEGIN BAR CHART;

    -- draw gdp of the top 10 countries
    CREATE SERIES WITH
      SELECT "gdp per country" as name gbp as x, country as y FROM gdp_per_country ORDER BY gdp DESC LIMIT 10;


Now lets add a custom style and axis deinitions:

  -- draw a simple bar chart
  DRAW BAR CHART
    WITH STYLE ...;

  -- draw gdp of the top 10 countries
  SERIES "gdp per country" FROM
    SELECT gbp, country FROM gdp_per_country ORDER BY gdp DESC LIMIT 10;
    WITH gdp AS X and country as Y;


Another example:

Draw a few latency distributions for multiple loadtest experiments sourced from
multiple csv files. The csv files contain two columns "time" and "runtime" and
one line for each request in the load test. The "time" column contains the wall
clock time at which the request was issued and the "runtime" column contains the
runtime of the request.

We will draw a single line graph that plots requests per minute vs the 90th
percentile latency in that time window.

    IMPORT experiment1 FROM "experiment_1.csv";

    DRAW LINE CHART;

    SERIES "experiment1" FROM
      SELECT count(*) as qps, nth_percentile(runtime, 90) as latency
        FROM experiment1
        GROUP BY time_window(time, 1s);

Lets add another experiment as a second series in the chart:

    IMPORT experiment1 FROM "experiment_1.csv";
    IMPORT experiment2 FROM "experiment_2.csv";

    BEGIN LINES;

    CREATE SERIES WITH SELECT
      "experiment1" as title, count(*) as x, nth_percentile(runtime, 90) as y
      FROM experiment1
      GROUP BY time_window(time, 1s);

    SERIES "experiment2" FROM
      SELECT count(*) as qps, nth_percentile(runtime, 90) as latency
        FROM experiment2
        GROUP BY time_window(time, 1s);


To make that last query more interesting lets say we put all our test results
into a single file with an additional "experiment" columns and try to recreate
the same chart:

    -- load a csv file with per request logs from a load test
    IMPORT loadtests FROM "myfile.csv";

    -- draw a graph of qps vs 90thpercentile, one series for each experiment
    BEGIN CHART;

    DRAW LINES experiment FROM
      SELECT experiment, count(*) as qps, nth_percentile(runtime) as latency
        FROM loadtests
        GROUP BY experiment, time_window(time, 1s);



