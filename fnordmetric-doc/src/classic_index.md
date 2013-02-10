The basic concept in FnordMetric is called a "gauge". A Gauge is a primitive data store for a numerical value over
time. A gauge has two dimensions: Value und Time. Gauges can be used in different modes like sum,
average, max/min, etcetara.

The FnordMetric core deals with processing data. There are two basic concepts:
events and gauges.

An Event is a piece of data that is sent to FnordMetric through one of the various
sources. This event is a JSON Object / Hash with almost no constraints on the schema.

You write event handlers in ruby that get invoked per incoming event and modify (increment,
set, etcetera) gauges.
