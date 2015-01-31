The GROUP OVER TIMEWINDOW clause
================================

Standard SQL makes (moving) window queries over timeseries fairly akward. Since
these kind of time based rollups are very common when working with timeseries data,
FnordMetric introduces a new GROUP OVER TIMEWINDOW clause.

---

The syntax for the GROUP OVER TIMEWINDOW clause is:

    GROUP OVER TIMEWINDOW(time_column, window_expr [, step_expr]) [BY ...]

Where `time\_col` is the column that contains the time for each row, `window\_expr`
is an expression that returns the length of the window in seconds and `step\_expr`
is an expression that returns the output timestep in seconds.

Here is a simple example that should demonstrate the general concept before we
dig into the detailed semantics of the GROUP OVER TIMEWINDOW clause:

_Display the moving average of mymetric's value over a moving 60s window_

    SELECT time, mean(value) FROM mymetric GROUP OVER TIMEWINDOW(time, 60);

---

### Detailed explanation

In contrast to a regular GROUP BY clause, which will produce one output row for
each distinct group in the input row set, the GROUP OVER TIMEWINDOW will produce
one output row for each step in time. Each of these output rows will be computed
by grouping over all rows in the preceding time window.

To illustrate this concept, lets imagine we insert the number of registered users
on our website every 10 seconds and our source table looks like this:

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
second step to compute a moving average

    SELECT time, mean(value) FROM ... GROUP OVER TIMEWINDOW(time, 40, 10)

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


GROUP OVER TIMEWINDOW rollup in multiple dimensions
---------------------------------------------------

Just like a normal GROUP BY clause, the GROUP OVER TIMEWINDOW clause accepts
a list of expressions by which to roll up the result.

As an example let's calculate the moving average of the number of requests
per hostname in the last hour:

    SELECT time, hostname, mean(value)
        FROM number_of_requests
        WHERE time > -1hour
        GROUP OVER TIMEWINDOW(time, 40, 10) BY hostname;

Or let's break down the 99th percentile latency by hostname and url over
a fixed time range:

    SELECT time, hostname, url, percentile(99, value)
        FROM request_latencies
        WHERE time > 2014-08-01 AND time < 2014-09-01
        GROUP OVER TIMEWINDOW(time, 40, 10) BY hostname, url;


GROUP OVER TIMEWINDOW on multiple tables
----------------------------------------

If a GROUP OVER TIMEWINDOW clause appears in a SELECT over multiple tables, the
tables are implicitly joined on the specified timewindow. This allows you to
easily align and join data from multiple tables/metrics.

An example:

    -- error rate computed from joining the first derivates of two metrics over a
    -- moving 60 second window in the last hour
    SELECT
      "error rate" as series,
      delta(my_success_count_metric) / delta(my_error_count_metric)
    FROM
      my_success_count_metric,
      my_error_count_metric
    WHERE time > -60minutes
    GROUP OVER TIMEWINDOW(time, 60);


GROUP OVER TIMEWINDOW semantics
-------------------------------

  - While a regular GROUP BY clause will produce one output row for each distinct
  group in the input row set, the GROUP OVER TIMEWINDOW will produce one output
  row for each step in time.

  - The number of output rows of a GROUP OVER TIMEWINDOW clause may be greater
  than the number of rows in the input set (if there is less than one input row
  per output time window).

  - The output groups may overlap, i.e. each input row may appear in more than
  one output group.

  - When selecting from multiple tables, a GROUP OVER TIMEWINDOW is an implicit
  natural join on the specified timewindow. GROUP OVER TIMEWINDOW clauses may
  therefore not be mixed with JOIN clauses.

