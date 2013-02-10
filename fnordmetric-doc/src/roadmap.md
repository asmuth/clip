
Todo
====



Backlog
=======

  → bignumberwidget
  → fancy topbar daterange picker for legacy dashboards
  → make fnordmetric responsive
  → favicon (F)
  → timeseries-widget: cardinal on/off should be flick-switch
  → namespace picker in sidebar
  → distgauge: render std. deviation
  → timeseries-gauge: overview series weights
  → timeseries-gauge: punchcards
  → incr_fraction: implement progressive
  → timeseries gauge: restrict to max resolution
  → formatter: num, time, currency, percent, time_of_day
  → minimize/pack js + css
  → put images into one sprite
  → nice empty states


Changelog
=========

  1.1.1
    → icons for sidebar
    → simple gauge explorer (style like active users) + html5 api snippet generator
    → new file structure
    → better namespacing for javascript

  1.1.0
    → js api (<div data-gauge="...">)
    → gauge human_titles, units, scale_by
    → added stomp acceptor, amqp acceptor, fyrehose acceptor
    → explicit dashboard initialization with options -> dashboard grouping
    → bugfix: lots of small bugfixes

  1.0.0
    → improved ui
    → highcharts is dead, long live d3.js :)
    → websockets instead of ajax
    → new gauges are variable interval!
    → new gauges: timeseries, toplist, distribution (anti-repetition!)
    → timeseries gauge: fraction api
    → new things: trends, punchards, stdeviation etc
    → splitted up code into acceptor, web, worker etc.
    → splitted up javascript code
    → refactorings, no server-side rendering
    → less ram usage / garbage collection / single tick
    → fnordmetric:announce redis pusub event-data instead of event_id
    → config in multiple files



Wiki
====

  The basic concept in FnordMetric is called a "gauge". A Gauge is a primitive data store for a numerical value over
  time. A gauge has two dimensions: Value und Time. Gauges can be used in different modes like sum,
  average, max/min, etcetara.

  The FnordMetric core deals with processing data. There are two basic concepts:
  events and gauges.

  An Event is a piece of data that is sent to FnordMetric through one of the various
  sources. This event is a JSON Object / Hash with almost no constraints on the schema.

  You write event handlers in ruby that get invoked per incoming event and modify (increment,
  set, etcetera) gauges.





  FnordMetric UI

    > getting started

    > api reference
      - counters
      - timeseries

    > examples
      - date / time controls
      - example_1.html


  FnordMetric Enterprises

    > getting started

    > http api

    > other data sourcs
      - stomp


  FnordMetric Classic

    > getting started

    > sending data 
      - _type, _time field mandatory
      - tcp, udp, redis, http, amqp, stomp
      - events containing user data (_session, _name)
      - _namespace

    > running fnordmetric
      - standalone
      - standalone w/ multiple workers
      - configuration options

    > event handlers
      - data, time, session etc methods
      - storing data in the session
      - end-of-session callback

   > gauge api
     - two vs three dimensional

  > custom dashboards
    - overview
   - incr, incr_field etc.

  > widgets
    - timerseries widget
    - numbers widget
    - bars widget

  > plugins
    > active users dashboard
    > gauge explorer
    > timeseries gauge
      - calculate-option: count, sum, progressive_sum, avg
      - incr / incr_counter / incr_denominator
    > toplist gauge
    > distribution gauge

  > hacking
    - custom views / widgets

  > examples
    - full_example.rb



