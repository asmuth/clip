ChartSQL Overview
=================

The ChartSQL query language is a superset of standard SQL. It isn't feature
complete yet, but it supports all the operations you would expect in your
standard SQL database like GROUP BY or HAVING statements and joins.

These are the main differences between ChartSQL and standard SQL:

The DRAW Statement
------------------

The DRAW statement allows you to specify that the query result should be returned
as a chart (or other visualization) rather than a table. Here is a simple example
of the DRAW statement:

    DRAW LINECHART
        WITH AXIS BOTTOM

    SELECT time AS x, value AS y
        FROM example_data;

Read the [documentation for the DRAW statement](/documentation/chartsql/draw_statement)


The IMPORT Statement
--------------------

The IMPORT statement allows you to import tables from External Data Sources,
like CSV files or a MySQL database. Here is a asmple axmple of the IMPORT
statement:

    IMPORT TABLE example_data
        FROM 'csv:examples/data/measurement.csv?headers=true';

    SELECT * FROM example_data;

Read the [documentation for the IMPORT statement](/documentation/chartsql/external_data_sources)


The GROUP OVER TIMEWINDOW Clause
--------------------------------

FnordMetric introduces a new GROUP OVER TIMEWINDOW clause to simplify time based
rollups when working with timeseries data. Here is a simple example of the GROUP
OVER TIMEWINDOW clause:

    -- compute a moving average with a 60 second window
    SELECT time, mean(value) FROM mymetric GROUP OVER TIMEWINDOW(60);

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
