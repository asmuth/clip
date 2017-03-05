Frequently Asked Questions
==========================

### Why is FnordMetric better than X?

One way to look at FnordMetric is that it's a SQL-based alternative to the
StatsD/Graphite, ELK or Prometheus stacks. So why would you choose FnordMetric
over these other solutions? Here are some reasons:

- The FnordMetric programs are just simple, stateless middleware services. All
  the heavy lifting is done by the backend database. You get to use the full
  power of a true RDBMS for your timeseries collection and querying.

- In the same vein, you don't have to learn another informally specified domain
  specific query language for your metrics/graphing tool. It's just SQL

- Also consider that since all your metrics data will be stored in a SQL database,
  you can easily JOIN it with other (non-timeseries) data tables, for example
  with records from your main production database.

- FnordMetric follows the unix philosophy: Do one thing and do it well. FnordMetric
  consists of two components `metric-queryd` and `metric-collectd` that are both
  narrow in scope.

- FnordMetric is implemented in self-contained C++. It's super quick to set up
  and does not require a lot of resources

- The FnordMetric project was started over 7 years ago and is still actively maintained


### What about monitoring and alerting?

FnordMetric contains components for capturing measurements into your SQL database
(metric-collectd) and for visualizing the results (metric-queryd). However, there
is no component for monitoring or alerting.

The reason is that the alerting rules are usually highly custom, so most other
systems include a small domain specific programming language to define the
alerting conditions.

We think that SQL is already a very well-suited language for this task and adding
any kind of rule engine on top would actually make it less powerful. In other words,
we think that we can't add a lot over plain old SQL for checking custom alerting
rules.

Hence, the recommendation for monitoring/alerting is to run all rules directly
against the SQL database. You could do this with a custom script that executes
the rule queries on the SQL database and connects with a third-party alerting
system PagerDuty or nagios.


### Is FnordMetric highly available/scalable/distributed?

All FnordMetric components are simple stateless services. So it all depends on
the backend database. If you're backend database is scalable and highly available
then so is FnordMetric.


### I'm getting build errors when compiling the code

FnordMetric needs a reasonably modern C++ compiler and standard library. In most
cases where the code doesn't build it's due to some problem with the local build
environment. We always appreciate bug reports so we improve our build system, but
often the best workaround for the moment is to download a binary release from the
downloads page.


### How do I import historical data?

Usually the best way to import historical data is to insert it directly into
the backend SQL database.

