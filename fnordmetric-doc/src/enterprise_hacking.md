
### In-memory vs. disk storage

FnordMetric Enterprise stores the values as 64bit double precision floats.

With an example flush timeout of 10 seconds, one metric uses 0.065 MB of
memory per day or 0.4 MB per week. The default ring buffer size is x,.

That means with only 4GB of ram, you could access the last month of data of
2500 counters/metrics with 10 second granularity all from the in-memory
ringbuffers (without ever hitting a HDD).

Requests that can not be served from memory require one sequential disk read.


### FnordMetric Enterprise vs. StatsD

+ allows for flush intervals as low as one second
+ rendered in the browser, interactive
+ much much more scalable
+ highly customizable with css
+ requires only a single deployment
+ i18n (proper timezones in graphs due to in browser rendering etc)
