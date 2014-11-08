ChartSQL Overview
=================

The ChartSQL query language is a superset of standard SQL. It isn't feature
complete yet, but it supports all the operations you would expect in your
standard SQL database like GROUP BY or HAVING statements and joins.

These are the main differences between ChartSQL and standard SQL:

#### The DRAW Statement

#### The IMPORT Statement

#### The GROUP OVER TIMEWINDOW Clause

FnordMetric introduces a new GROUP OVER TIMEWINDOW clause to simplify time based
rollups when working with timeseries data. Here is a simple example of the GROUP
OVER TIMEWINDOW clause:

    SELECT time, mean(value) FROM mymetric GROUP OVER TIMEWINDOW(60, 10);

Read the [documentation for the GROUP OVER TIMEWINDOW clause](/documentation/chartsql/timewindow_aggregations)


Other Extensions to standard SQL
--------------------------------

#### GROUP BY and HAVING extensions

Like MySQL, fnordmetric SQL extends the use of GROUP BY so that the select list
can refer to nonaggregated columns not named in the GROUP BY clause. This assumes
that the nongrouped columns will have the same group-wise values. Otherwise, the
result is undefined. The same applies for the HAVING clause.

If you use a group function in a statement containing no GROUP BY clause, it
will emit a single group.
