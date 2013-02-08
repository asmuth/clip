FnordMetric v1.0
================


Todos
=====

  → bugfix: widgets w/o gauges
  → simple gauge explorer (style like active users) + iframe snippet generator
  → linux/server monitor gauge (http://www.crunchbase.com/assets/images/original/0010/3221/103221v2.png)+ 
  → exceptions gauge
  → icons for sidebar


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


Done
====

  → js api (<div data-gauge="...">)
  → gauge human_titles, units, scale_by
  → added stomp acceptor, amqp acceptor, fyrehose acceptor
  → explicit dashboard initialization with options -> dashboard grouping
  → bugfix: lot's of small bugfixes


Wiki
====

  FnordMetric Core

    > disambiguation: Concepts && Two ways to use: web interface vs. embed (like graphite)
    > quick start

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
     - incr, incr_field etc.


  FnordMetric Embed

    > quickstart

    > api reference
      - counters
      - timeseries

    > examples
      - full_example.rb


  FnordMetric Frontend

    > custom dashboards
      - overview

    > widgets
      - timerseries widget
      - numbers widget
      - bars widget

    > pre-made gauges
      > timeseries gauge
        - calculate-option: count, sum, progressive_sum, avg
        - incr / incr_counter / incr_denominator
      > toplist gauge
      > distribution gauge

    > hacking
      - custom views / widgets

    > examples
      - full_example.rb





IDEAS
-----

  > overview view: gauge list (a'la github graphs landing)
  > overview view: num active users, system stats (realtime widget)
  > special events (incr, observe etc)
  > distgauge: histograms tab
  > toplgauge: tabs: compare
  > timeseries-gauge: cmp. w/yesterday option
  > geo_distribution_gauge
  > value correlation / scatter plot (2d) gauge




CHANGELOG
---------

  > improved ui
  > highcharts is dead, long live d3.js :)
  > websockets instead of ajax
  > new gauges are variable interval!
  > new gauges: timeseries, toplist, distribution (anti-repetition!)
  > timeseries gauge: fraction api
  > new things: trends, punchards, stdeviation etc
  > splitted up code into acceptor, web, worker etc.
  > splitted up javascript code
  > refactorings, no server-side rendering
  > less ram usage / garbage collection / single tick
  > fnordmetric:announce redis pusub event-data instead of event_id
  > config in multiple files

