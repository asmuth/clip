FnordMetric
===========

FnordMetric is a highly configurable (and pretty fast) realtime app/event tracking thing based on ruby eventmachine and redis. You define your own plotting and counting functions as ruby blocks!

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/fnordmetric.png) ](http://travis-ci.org/paulasmuth/fnordmetric) [ ![Dependency status - Gemnasium](https://gemnasium.com/paulasmuth/fnordmetric.png) ](https://gemnasium.com/paulasmuth/fnordmetric)

[SCREENCAST](http://www.screenr.com/KiJs): the FnordMetric-instance we use to track our social dating app.

----

FnordMetric keeps track of your data and draws nice timeline plots.

[ ![Nice timeline plots](https://raw.github.com/paulasmuth/fnordmetric/master/doc/preview1.png) ](https://raw.github.com/paulasmuth/fnordmetric/master/doc/preview1.png)

FnordMetric gives you a live dashboard, that shows who is using your app in realtime. You can select a single user and follow them step by step.

[ ![Live dashboard](https://raw.github.com/paulasmuth/fnordmetric/master/doc/preview2.png) ](https://raw.github.com/paulasmuth/fnordmetric/master/doc/preview2.png)


Getting Started
---------------

Copy `doc/ulm_stats.rb` (that's the configuration from the screenshots and screencast) or the simple example from below to `my_stats_app.rb`.

Simple Example: this will listen for json-events with `type=unicorn_seen` and render a timeline-plot showing the number of received events per hour.

```ruby
require "fnordmetric"

FnordMetric.namespace :myapp do

# numeric (delta) gauge, 1-hour tick
gauge :unicorns_seen_per_hour, 
  :tick => 1.hour.to_i, 
  :title => "Unicorns seenper Hour"

# on every event like { _type: 'unicorn_seen' }
event(:unicorn_seen) do
  # increment the unicorns_seen_per_hour gauge by 1
  incr :unicorns_seen_per_hour 
end

# draw a timeline showing the gauges value, auto-refresh every 2s
widget 'Overview', {
  :title => "Unicorn-Sightings per Hour",
  :type => :timeline,
  :gauges => :unicorns_seen_per_hour,  
  :include_current => true,
  :autoupdate => 2
}

end

FnordMetric.standalone
```

Start the app (requires ruby >= 1.9.2):

    $ ruby my_stats_app.rb run

Log all incoming events:

    $ ruby my_stats_app.rb log DUMP_FILE=/tmp/foo.json

This is the easiest way to submit an event:

    echo "{\"_type\": \"unicorn_seen\"}" | nc localhost 1337


Installation
------------

    gem install fnordmetric

or in your Gemfile:

    gem 'fnordmetric', '~> 0.5'


Documentation
-------------

### Sending Events ###

The slow way: HTTP-Post the json event to the fnordmetric webinterface.

    POST http://localhost:2323/events _type=unicorn_seen

    curl -X POST -d "_type=unicorn_seen" http://localhost:4242/events 

The easy way: Stream one ore more newline-seperated json encoded events through a tcp connection.

    echo "\{\"_type\": \"unicorn_seen\"\}\n" | nc localhost 2323

The fast way: Add your event directly to the redis-based queue.

```ruby
uuid = (8**32).to_s(36)
event = { :_type => "unicorn_seen" }.to_json

redis.set("fnordmetric-event-#{my_uuid}", event)
redis.expire("fnordmetric-event-#{my_uuid}", 60)
redis.lpush("fnordmetric-queue", uuid) 
```

----

### Special Events ###

```js
// track a pageview
{ "_type": "_pageview", "url": "/blob/my_super_seo_article", "_session": "mysessiontoken" }

// set the user name
{ "_type": "_set_name", "name": "Tingle Tangle Bob", "_session": "mysessiontoken" }

// set the user picture
{ "_type": "_set_picture", "url": "http://myhost/123.jpg", "_session": "mysessiontoken" }
```

----

### Event Handlers ###

Call these methods from the event-handler block

    incr(gauge_name, value=1): 
      Increment the given (two-dimensional) gauge by value at the tick specified by event-time

    incr_field(gauge_name, field_name, value=1): 
      Increment the given field on a three-dimensional gauge by value at the tick specified by event-time

    set_value(gauge_name, value)
      Set the given (two-dimensional) to value at the tick specified by event-time (overwrite existing value)

    set_field(gauge_name, field_name, value)
      Set the given  field on a three-dimensional gauge to value at the tick specified by event-time (overwrite existing value)

----

### Options: Gauges ###

----

### Options: Widgets ###

+ `[autoupdate]` auto-refresh the timeline every n secs (0 turns autoupdate off)

TimelineWidget

+ `[plot_style]` one of: line, areaspline
+ `[include_current]` show the current tick?
+ `[ticks]` number of ticks to show (defaults to 24/30)

BarsWidget

+ `[plot_style]` one of: vertical, horizontal
+ `[order_by]`: order bars/columns by: value, field


----

### JSON API ###


Examples
--------

+ doc/ulm_stats.rb


## Full Example ##

```ruby
require "fnordmetric"

FnordMetric.namespace :myapp do

  # numeric (delta) gauge, 1-hour tick
  gauge :messages_sent, 
    :tick => 1.hour.to_i, 
    :title => "Messages (sent) per Hour"

  # numeric (delta) gauge, 1-hour tick
  gauge :messages_read, 
    :tick => 1.hour.to_i, 
    :title => "Messages (read) per Hour"

  # numeric (progressive) gauge, 1-hour tick
  gauge :events_total, 
    :tick => 1.day.to_i, 
    :progressive => true,
    :title => "Events (total)"

  # numeric (delta) gauge, increments uniquely by session_key
  gauge :pageviews_daily_unique, 
    :tick => 1.day.to_i, 
    :unique => true, 
    :title => "Unique Visits (Daily)"

  # numeric (delta) gauge, increments uniquely by session_key, returns average
  gauge :avg_age_per_session, 
    :tick => 1.day.to_i, 
    :unique => true,
    :average => true,
    :title => "Avg. User Age"

  # three-dimensional (delta) gauge (time->key->value)
  gauge :pageviews_per_url_daily, 
    :tick => 1.day.to_i, 
    :title => "Daily Pageviews per URL", 
    :three_dimensional => true


  # on every event like { "_type": "message_sent" }
  event(:message_sent) do
    # increment the messages_sent gauge by 1
    incr :messages_sent 
  end

  # on every event like { "_type": "message_read" }
  event(:message_read) do 
    # increment the messages_read gauge by 1
    incr :messages_read 
  end

  # on _every_ event
  event :"*" do
    # increment the events_total gauge by 1
    incr :events_total
  end

  # on every event like { "_type": "_pageview", "_session": "sbz7jset", "url": "/page2" }
  event :_pageview do
    # increment the daily_uniques gauge by 1 if session_key hasn't been seen in this tick yet
    incr :pageviews_daily_unique
    # increment the pageviews_per_url_daily gauge by 1 where key = 'page2'
    incr_field :pageviews_per_url_daily, data[:url]
  end

  # on every event like { "_type": "_my_set_age", "my_age_field": "23" }
  event(:my_set_age) do 
    # add the value of my_set_age to the avg_age_per_session gauge if session_key 
    # hasn't been seen in this tick yet
    incr :avg_age_per_session, data[:my_age_field] 
  end

  # draw a timeline showing the pageviews_daily_unique, auto-refresh every 30s
  widget 'Overview', {
    :title => "Unique Visits per Day",
    :type => :timeline,
    :width => 70,
    :gauges => :pageviews_daily_unique,
    :include_current => true,
    :autoupdate => 30
  }

 # draw the values of the messages_sent and messages_read gauge at the current tick, three ticks ago, and
 # the sum of the last 10 ticks, auto-refresh every 20s
 widget 'Overview', {
    :title => "Messages Sent / Read",
    :type => :numbers,
    :width => 30,
    :autoupdate => 20,
    :offsets => [0,3,"10s"],
    :gauges => [ :messages_sent, :messages_read ]
  }

  # draw a list of the most visited urls (url, visits + percentage), auto-refresh every 20s
  widget 'Overview', {
    :title => "Top Pages",
    :type => :toplist,
    :autoupdate => 20,
    :gauges => [ :pageviews_per_url_daily ]
  }

end

FnordMetric.standalone
```

Contributors
------------

+ Simon Menke (http://github.com/fd)
+ Bruno Michel (http://github.com/nono)
+ Marco Borromeo (http://github.com/mborromeo)
+ Leo Lou (http://github.com/l4u)
+ Andy Lindeman (http://github.com/alindeman)

To contribute, please fork this repository, make your changes and run the specs, commit them to your github repository and send me a pull request.


License
-------

Copyright (c) 2011 Paul Asmuth

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to use, copy and modify copies of the Software, subject 
to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


Todos
-----

* funnel-widget
* combine/calculation gauges via opts_gauge({}, &block) (+calculate ctr)
* timeline_widget: 'compare mode': compate gauge to yesterday
* numbers_widget: handle decreasing vals
* make listen-ports configurable
* referal tracking fu (parse googlequeries)  
* trend detection
* opt_event options: :increment => gauge_name
* preconfigured default-dashboard (like google analytics)
* pagview+ref-tracking via js-tracking-pixel 
* table/gauge-list-widget (with mini-stats!)
* prune the namespace-sessions-timline (remove event_ids older than x)
* prune the namespace-event-types-list (trim to max items)
* timelinewidget + numberswidget => should use redis hmget
* get multiple metrics in a single http get
* `{ _namespace: myns }` field 
