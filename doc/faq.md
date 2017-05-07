Frequently Asked Questions
==========================

### What about monitoring and alerting?

metrictools contains commands for capturing measurements and for visualizing the
data. However, there are no commands specifically inteded for monitoring or alerting.

The recommendation for monitoring/alerting is to use an external system that
reads the metric data and calculates the alarm states. You could do this with a
custom script or a third-party alerting system like nagios.


### Is metrictools highly available/scalable/distributed?

All metrictools components are simple stateless services. So it all depends on
the backend database. If you're backend database is scalable and highly available
then so is metrictools.


### I'm getting build errors when compiling the code

metrictools needs a reasonably modern C++ compiler and standard library. In most
cases where the code doesn't build it's due to some problem with the local build
environment. We always appreciate bug reports so we improve our build system, but
often the best workaround for the moment is to download a binary release from the
downloads page.

