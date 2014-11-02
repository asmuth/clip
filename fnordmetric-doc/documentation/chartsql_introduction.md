ChartSQL Overview
=================

I would love to say that the FnordMetric query language is a superset of SQL99,
but we aren't there just yet.

It supports all the operations you would expect in your standard SQL database
like GROUP BY or HAVING statements and joins. Additionally, FnordMetric implements
a number of new statements to create charts directly from SQL.

Extensions to standard SQL
--------------------------

### GROUP BY and HAVING extensions

Like MySQL, fnordmetric SQL extends the use of GROUP BY so that the select list
can refer to nonaggregated columns not named in the GROUP BY clause. If you use
a group function in a statement containing no GROUP BY clause, it equivalent to
grouping on all rows. This assumes that the nongrouped columns will have the same
group-wise values. Otherwise, the result is undefined. The same applies for
the HAVING clause.
