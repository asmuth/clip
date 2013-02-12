Events and Gauges
-----------------

A "gauge" in FnordMetric is basically a bucket that stoes a numerical value. It has has two
dimensions: time and value. Each gauge is identified by a uniqe key (for example
"number_of_singups_per_minute"). The value of a gauge is periodically aggregated and persisted
into redis.

Gauges can be used in different modes: They can act as simple counters with an increment and
a decrement operation, but you can also use them to record the mean / average or the max/min
value.

An Event is a piece of input data that is sent to FnordMetric through one of the various
sources. These events are JSON objects (arbitrary hashmaps) with almost no contraints on
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
you to track what a specific user is doing, more about that in Sessions FIXPAUL). The
picture and name are displayed e.g. in the "Active Users" Plugin (FIXPAUL)

    { "_type": "_set_name", "_session": "A7697BA0939C02E ", "name": "Teddy Tester" }

FIXPAUL: Reference


### Custom Events

You can also send completely custom events and write a piece of ruby code (called
the "event handler") that tells FnordMetric what to do with them.

In this case the only requirement is that the event has a `_type` key, which is used
to identify the correct event handler. This is useful if you want to perform more than
one action but only want to send one piece of data from your app to FnordMetric.

#### Example: send a "user logged in up" event and increment two gauges

First we need to create a gauge. When using the predefined _incr, _decr, etc.. events
gauges are created on-the-fly, but when writing custom event handlers we need to do
this by hand. We create a gauge that stores the number of logins with a granularity (`flush_interval`)
of 1 minute and a another gauge that stores the number of total conversions per day.

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
and `incr` that we can use to access the event data and mofiy gauges. More info about that
in the Reference FIXPAUL.

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

    fixpaul: special key table
    - events containing user data (_session, _name)
    - _namespace


### Gauges

FIXPAUL gauge options reference

#### Average Gauges

#### Progressive Gauges

#### Three-dimensional Gauges



### API Reference

#### Accessing the Event Data
  - data, time, etc

#### incr, etcetera

#### Sessions
FnordMetric allows you to track what a specific user is doing
  - storing data in the session
  - end of session callback
  - set_name/picture => above

