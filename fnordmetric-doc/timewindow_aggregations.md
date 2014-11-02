

### The GROUP OVER TIMEWINDOW clause

Standard SQL makes (moving) window queries over timeseries fairly akward. Just
computing a moving average requires complex self-joins.

Since these kind of time based rollups are very common when working with
timeseries data, FnordMetric introduces a new GROUP OVER TIMEWINDOW clause.

Here is a simple example that should demonstrate the general conpect before we
look at the detailed semantics of the GROUP OVER TIMEWINDOW clause:

    -- display the moving average of mymetric's value over a mving 60s window
    SELECT time, mean(value) FROM mymetric GROUP OVER TIMEWINDOW(60, 10);

### Detailed explanation

In contrast to a regular GROUP BY clause, which will produce one output row for
each distinct group in the input row set, the GROUP OVER TIMEWINDOW will produce
one output row for each step in time. Each of these output rows will be computed
by grouping over all rows in the preceeding time window.

To illustrate this concept, lets imagine we insert the number of registered users
on our website every 10 seconds. This could produce a raw table that looks like
this:

     time   | value
     ----------------
     0      | 534
     10     | 563
     20     | 598
     30     | 632
     40     | 732
     50     | 887
     60     | 923
     70     | 987
     80     | 992
     90     | 999

Now let's GROUP OVER TIMEWINDOW on this table with a 40 second window and a 10
second step to produce a moving average

    SELECT time, mean(value) FROM ... GROUP OVER TIMWINDOW(40, 10)

The GROUP OVER TIMEWINDOW clause will produce one output row each 10 seconds,
grouping over the last 40 seconds of data. This illustration shows the first
four windows:

     time   | 0s  | 10s | 20s | 30s | 40s | 50s | 60s | 70s | 80s | 90s |
     value  | 543 | 563 | 598 | 632 | 732 | 887 | 923 | 987 | 992 | 999 |

            <------ window 1 ------->
                  <------ window 2 ------->
                         <------ window 3 ------->
                              <------ window 4 ------->


As you can see above, every input row can appear in more than one output group.
Our example query will produce the following output:

     time   | mean(value)
     ---------------------
     30     | ...           --- window 1: 543 + 563 + 598 + 632 / 4
     40     | ...           --- window 2: 563 + 598 + 632 + 732 / 4
     50     | ...           --- window 3: 598 + 632 + 732 + 887 / 4
     60     | ...           --- window 4: 632 + 732 + 887 + 923 / 4
     70     | ...
     80     | ...
     90     | ...


### GROUP OVER TIMEWINDOW rollup in multiple dimensions

Just like a normal GROUP BY clause, the GROUP OVER TIMEWINDOW clause accepts
a list of expressions by which to roll up the result.

As an example let's calculate the moving average of the number of requests
per hostname:

    SELECT time, mean(value)
        FROM number_of_requests
        GROUP OVER TIMWINDOW(40, 10) BY hostname;

Or let's break down the 99th percentile latency by hostname and url:

    SELECT time, percentile(99, value)
        FROM request_latencies
        GROUP OVER TIMWINDOW(40, 10) BY hostname, url;


### GROUP OVER TIMEWINDOW on multiple tables

If a GROUP OVER TIMEWINDOW clause appears in a SELECT over multiple tables, the
tables are implicitly joined on the specified timewindow. This allows you to
easily align and join data from multiple tables/metrics.

An example:

    -- error rate computed from joining the first derivates of two metrics over a
    -- moving 60 second window in the last hour, move the window in 10s steps
    SELECT
      "error rate" as series,
      delta(my_success_count_metric) / delta(my_error_count_metric)
    FROM
      my_success_count_metric,
      my_error_count_metric
    WHERE time > -60minutes
    GROUP BY TIMEWINDOW(60, 10);


### GROUP OVER TIMEWINDOW semantics

  - While a regular GROUP BY clause will produce one output row for each distinct
  group in the input row set, the GROUP OVER TIMEWINDOW will produce one output
  row for each step in time.

  - A GROUP OVER TIMEWINDOW clause may produce more than one output row for each
  row in the input set (if there is less than one input row per time window).

  - When selecting from multiple tables, a GROUP OVER TIMEWINDOW is an implicit
  natural join on the specified timewindow. GROUP OVER TIMEWINDOW clauses may 
  therefore not be mixed with JOIN clauses.







---------------------- work in progress below here -----------------------



### More examples:

  -- error rate computed from joining the first derivates of two metrics over a
  -- moving 60 second window in the last hour
  SELECT
    "http error rate" as series,
    num_200.time as x,
    num_500.val / num_200.val as y
  FROM (
      SELECT time, delta(value) AS val
        FROM http_status_codes
        SINCE -60minutes UNTIL now
        WHERE status_code = 200
        GROUP BY TIMEWINDOW(time, 60, 10);
    ) AS num_200
  JOIN (
      SELECT time, delta(value) AS val
        FROM http_status_codes
        SINCE -60minutes UNTIL now
        WHERE status_code = 5200
        GROUP BY TIMEWINDOW(time, 60, 10);
    ) AS num_500
  ) ON num_200.time = num_500.time;



  -- error rate computed from joining the first derivates of two metrics over a
  -- moving 60 second window in the last hour
  SELECT
    "http error rate" as series,
    num_200.time as x,
    num_500.val / num_200.val as y
  FROM (
      SELECT time, delta(value) AS val
        FROM http_status_codes
        SINCE -60minutes UNTIL now
        WHERE status_code = 200
        GROUP BY TIMEWINDOW(time, 60, 10);
    ) AS num_200, (
      SELECT time, delta(value) AS val
        FROM http_status_codes
        SINCE -60minutes UNTIL now
        WHERE status_code = 5200
        GROUP BY TIMEWINDOW(time, 60, 10);
    ) AS num_500
  WHERE num_200.time = num_500.time;





-- insert distribution as map of $bucket -> $count pairs. expand each $bucket to
-- $count rows with value = $bucket on query
-- e.g. insert value {10:2, 100: 4, 250: 1}
-- select...
-- time, value
-- t0, 10
-- t0, 10
-- t0, 100
-- t0, 100
-- t0, 100
-- t0, 100
-- t0, 250


-- display the last hour of measurements
SELECT "mymetric" as series, time as x, value as y FROM mymetric,

-- display the moving average of our measurement over a moving 60s window
SELECT time as x, average(value) as FROM mymetric GROUP BY TIMEWINDOW(time 60, 10);


-- insert rationals/fractions (e.g. error rate)
-- allows proper aggregation over error rate:
-- display the aggregate error rate with a moving 60s window in the last hour
SELECT time as x, sum(numerator(value)) / sum(denominator(value)) as y
  FROM mymetric
  GROUP BY TIMEWINDOW(time, 60, 10)
  WHERE time > -60mins;

-- display the error rate per host with a moving 60s window in the last hour
SELECT time as x, sum(numerator(value)) / sum(denominator(value)) as y
  FROM mymetric
  GROUP BY TIMEWINDOW(time, 60, 10), hostname
  WHERE time > -60mins;

-- display the first derivative of our measurement over a moving 60s window in the last hour
SELECT time as x, delta(value) as y
  FROM mymetric
  GROUP BY TIMEWINDOW(time, 60, 10)
  WHERE time > -60mins;

-- ....equivalent to
SELECT time as x, delta(value) as y
  FROM mymetric
  GROUP BY TIMEWINDOW(time, 60, 10)
  SINCE -60mins UNTIL now;

-- number of requests per http status code over a moving 60 second window in
-- the last hour
SELECT status_code as series, time as x, delta(value) as y
  FROM http_status_codes
  GROUP BY TIMEWINDOW(time, 60, 10), status_code;

-- error rate computed from joining two metrics over a moving 60 second window
-- in the last hour
SELECT
  hostname as series,
  time as x,
  delta(error_metric.value) / delta(success_metric.value) as y
FROM
  success_metric,
  error_metric
WHERE
  time > -60minutes
GROUP BY
  TIMEWINDOW(time, 60, 10),
  hostname;


