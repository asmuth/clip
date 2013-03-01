Events and Gauges
-----------------

A "gauge" in FnordMetric is basically a bucket that stores a numerical value. It has has two
dimensions: time and value. Each gauge is identified by a uniqe key (for example
`number_of_singups_per_minute`). The value of a gauge is periodically aggregated and persisted
into redis.

Gauges can be used in different modes: They can act as simple counters with an increment and
a decrement operation, but you can also use them to record the mean/average or the max/min
value.

An Event is a piece of input data that is sent to FnordMetric through one of the various
sources. These events are JSON objects (arbitrary associative maps) with almost no contraints on
the schema. A event may look like this:

    { "_type": "sale", "product_id": 534221, "purchase_value": 2999 }

You can find more information about the various ways to submit these events to FnordMetric
(HTTP, TCP/UDP, etc.) in [Sending Data](/documentation/classic_sending_data)


### Predefined events

Some events have a special format and meaning (like "increment this counter by 4"
or "add this sample to the average query time gauge"). You don't have to write any
custom code to use these predefined events: Just send them (see [Sending Data](/documentation/classic_sending_data))
and FnordMetric will know what to do.
<br /><br />

#### Predefined Events: _incr, _decr, _set, _avg, _min, _max

These events increment, decrement or set counters or add a sample to a mean/avg, min or max
value gauge. You don't have to create the gauges upfront, they are automatically created
as you send events (if they don't exist yet).

Example: _Increment the gauge `sales-per-second` by 4 (gauge will be create if it doesn't exist)_

    { "_type": "_incr", "value": 4, "gauge": "sales-per-second" }

Example: _Sample average request time (this sample: 42ms), gauge resolution is 1 second._

    { "_type": "_avg", "value": 42, "gauge": "avg-request-time", "flush_interval": 1 }


Valid keys for these events are:

<table>
  <tr>
    <th><b>_type</b> <i>(mandatory)</i></th>
    <td>
      the operation to perform

      <p>allowed values are `_incr`, `_decr` and `_set` to
      increment, decrement and set counters and `_avg`, `_min`, `_max` to sample
      average, minimum or maximum values.</p>
    </td>
  </tr>
  <tr>
    <th><b>gauge</b> <i>(mandatory)</i></th>
    <td>
      name of the gauge. if it does not exist it will be created.
    </td>
  </tr>
  <tr>
    <th><b>value</b> <i>(mandatory)</i></th>
    <td>
      the value as integer
    </td>
  </tr>
  <tr>
    <th><b>flush_interval</b></i></th>
    <td>
      interval in which the value of this gauge is aggregated persisted in seconds.
      this is basically the gauge's granularity / resolution. the flush interval
      controls how much memory a gauge uses (smaller flush_intervals use more memory)
      the default is 10 seconds
    </td>
  </tr>
</table>
<br /><br />


#### Predefined Events: _set_name, _set_picture, _pageview

These events allow you the set a picture and display name for a visitor / user. They
require a `_session` key to identify a particular user session (FnordMetric allows
you to track what a specific user is doing). The picture and name are displayed e.g.
in the [Active Users Plugin](/documentation/classic_plugins)

    // track a pageview
    { "_type": "_pageview", "url": "/blob/my_super_seo_article", "_session": "mysessiontoken" }

    // set the user name
    { "_type": "_set_name", "name": "Tingle Tangle Bob", "_session": "mysessiontoken" }

    // set the user picture
    { "_type": "_set_picture", "url": "http://myhost/123.jpg", "_session": "mysessiontoken" }


### Custom Events

You can also send completely custom events and write a piece of ruby code (called
the "event handler") that tells FnordMetric what to do with them.

In this case the only requirement is that the event has a `_type` key, which is used
to identify the correct event handler. This is useful if you want to perform more than
one action but only want to send one piece of data from your app to FnordMetric.

#### Example: send a "user logged in up" event and increment two gauges

First we need to create a gauge. When using the predefined events (like `\_incr` and `\_avg`)
gauges are created on-the-fly, but when writing custom event handlers we need to do
this by hand. We create a gauge that stores the number of logins with a granularity
of 1 minute (this is called the `flush_interval`) and a another gauge that stores the number
of total conversions per day.

    gauge :logins_per_minute,
      :tick => 1.minute.to_i,
      :title => "Logins per Minute"

    gauge :conversions_per_day,
      :tick => 1.day.to_i,
      :title => "Conversions per day"

The event that we will send to FnordMetric looks like this (the schema can be completely
custom, this is just an example):

    { "_type": "login", "user_id": 1231, "user_name": "Teddy Tester" }

We tell FnordMetric what to do with this event by writing an event handler: (FnordMetric
uses the `_type` key to lookup theevent handler). There a are a few DSL methods like `data`
and `incr` that we can use to access the event data and mofiy gauges (more about that below).

    event :login do
      puts "user #{data[:user_name]} logged in"
      incr :logins_per_minute, 1
      incr :conversions_per_day, 1
    end

#### Catch-all event handler

You can register a event handler to the `*` type/key to have it executed for every incoming
event:

    event :"*" do
      puts "received event: #{data.inspect}"
    end


#### Special Keys

There is a small number of keys which have a special meaning, all of them
are prefixed with an underscore:

<table>
  <tr>
    <th>_type <i>(mandatory)</i></th>
    <td>
      the type of this event. this key is the only one that is mandatory as it is used to
      look up the correct event handler.
    </td>
  </tr>
  <tr>
    <th>_time</th>
    <td>
      contains the time at which this event was recorded as a unix timestamp. you can
      retroactively add events by setting this to a value in the past. (this field is optional)
    </td>
  </tr>
  <tr>
    <th>_session</th>
    <td>
      contains the session id of the user that triggered this event (optional)
    </td>
  </tr>
  <tr>
    <th>_namespace</th>
    <td>
      sets the namespace for this event (optional, you only need this if you have more than
      one namespace)
    </td>
  </tr>
</table>

### Gauges

The normal gauge is just a counter that you can increment or decrement. But there
are other modes to use gauges:

#### Average Gauges

One common is case is that you dont want to sum up your samples but to calculate
the mean / average. For example to record the "average response time". You can
do this by setting the `:average` key to true:

_Example: measure the average response time, this sample: 42ms_

    gauge :average_response_time_in_ms,
      :average => true

    event :request_finished
      incr :average_response_time_in_ms, 42
    end


#### Progressive / Cumulative Gauges

Sometimes you want to record a value not per-time but the total. For example "total
signed up users since launch". You can use progressive / cumulative gauges to do this,
just set the `:progressive` key to true:

_Example: record a signup_

    gauge :total_signups_since_launch,
      :progressive => true

    event :signup do
      incr :total_signups_since_launch 1
    end


#### Three-dimensional Gauges

While regular gauges have two dimensions (value and time) you can also have three dimensional
gauges (label, value and time) to record things like "clicks per category". These three dimensional
gauges basically act like a associative map were each entry is a simple two dimensional gauge.

_Example: record 2 clicks in the category 'fashion'_

    gauge :clicks_per_category,
      :three_dimensional => true

    event :click do
      incr_field :three_dimensional, 'fashion', 1
    end


#### All options

This is the full list of valid configuration options for gauges:

<table>
  <tr>
    <th>title</i></th>
    <td>
      sets the title of this gauge (optional)
    </td>
  </tr>
  <tr>
    <th><b>flush_interval</b></i></th>
    <td>
      interval in which the value of this gauge is aggregated persisted in seconds.
      this is basically the gauge's granularity / resolution. the flush interval
      controls how much memory a gauge uses (smaller flush_intervals use more memory)
      the default is 10 seconds
    </td>
  </tr>
  <tr>
    <th>average</i></th>
    <td>
      if set to true, this gauge will record the average value (see above)
    </td>
  </tr>
  <tr>
    <th>progressive</i></th>
    <td>
      if set to true, this gauge will record the cumulative value (see above)
    </td>
  </tr>
  <tr>
    <th>three_dimensional</i></th>
    <td>
      if set to true, this gauge acts as a associative map of counters (see above)
    </td>
  </tr>
</table>
<br /><br />



### Event Handler API

This is the list of methods that are available from within an event handler
(you can of course also use any custom ruby code)

#### Accessing the event data

When called from inside of an event handler, these methods return
the data / time / type of the current event.

<table style="margin-bottom:15px;">
  <tr>
    <th>data <i>=> Hash</i></th>
    <td>
      returns the event as a ruby hash. all keys are symbolized (e.g. <br /> `{ :_type => "my_event" ... }`
    </td>
  </tr>
  <tr>
    <th>time <i>=> Int</i></th>
    <td>
      return the time this event was registered at as a unix timestamp / integer
    </td>
  </tr>
  <tr>
    <th>type <i>=> Symbol</i></th>
    <td>
      returns the type of this event as a symbol (the content of the "_type" key)
    </td>
  </tr>
  <tr>
    <th>session_key <i>=> String or nil</i></th>
    <td>
      returns the session id of this event if set (the content of the "_session" key)
    </td>
  </tr>
</table>

_Example:_

    >> data
    => { :_type => "my_event", :my_value => 123 }

    >> time
    => 1360623178


#### Manipulating gauges

These methods allow manipulation of gauges. The time / bucket to modify
is automatically inferred from the content of the `_time` key.

<table style="margin-bottom:15px;">
  <tr>
    <th>incr(gauge, value = 1)</th>
    <td>
      increments the gauge by `value`. if the gauge is configured to be average / mean it records the value as one sample.
    </td>
  </tr>
  <tr>
    <th>incr_field(gauge, field, value = 1)</th>
    <td>
      can only be used on three-dimensional gauges. increments the label / field of the gauge by `value`.
    </td>
  </tr>
  <tr>
    <th>set_value(gauge, value)</th>
    <td>
      sets the value of this gauge to `value`
    </td>
  </tr>
  <tr>
    <th>set_field(gauge, field, value)</th>
    <td>
      can only be used on three-dimensional gauges. sets the value of this label / field of this gauge to `value`
    </td>
  </tr>
</table>

_Examples:_

    # increment the gauge 'total_sales' by 4
    incr :total_sales, 4

    # set the gauge 'users_online' to 5321
    set_value :users_online, 5321

    # increment the label 'clothing' on the three dimensional gauge 'top_categories' by 2
    incr :top_categories, 'clothing', 2

<br />

Fore more information check out the [Full Ruby DSL Example](/documentation/examples/fm_classic_full_example)
