Sending Data
------------

An Event is a piece of input data that is sent to FnordMetric through one of the various
sources. These events are JSON objects (arbitrary hashmaps) with almost no contraints on
the schema. A event may look like this:

    { "_type": "sale", "product_id": 534221, "purchase_value": 2999 }


This page describes how to send these events to FnordMetric. To read more
about the semantics of the events please see Events and Gauges (FIXPAUL)

You can choose between a variety of ways to submit these events to FnordMetric:

### HTTP API

The canonical way to submit events is using JSON + HTTP POST. Assuming your
FnordMetric Web Interface is running on port 4242, the URL of the api endpoint
is:

    POST http://localhost:4242/events

_Example: send a test event with cURL_

    curl -X POST -d '{ "_type": "test" }' localhost:4242/events


### TCP / UDP API

FnordMetric includes a TCP and a UDP Listener. They allow you to stream events
with minimal overhead, and in the case of UDP in a "fire and forget" fashion.

The Protocol is quite simple: One JSON Object per line. The JSON Object itself
is not allowed to include newlines (the JSON spec forbids this aswell).

With TCP you simply open a connection and start streaming events seperated by
newline. With UDP you can include one or more events in a packet, also seperated
by newline.

A Frame looks like this

    { "_type": "event_fnord", ... }\n
    { "_type": "event_fu", ... }\n
    { "_type": "event_bar", ... }\n

Start the TCP or UDP Acceptor in your FnordMetric config (see [Running FnordMetric](/documentation/classic_running_fm))

_Example: start a TCP Accepor on port 2323_

    FnordMetric::Acceptor.new(:protocol => :tcp, :port => 2323)

_Example: send a single event over TCP with netcat_

    echo '{ "_type": "test" }' | nc localhost 2323


### Redis API

You can put your events directly into FnordMetric's internal queue in redis
with the redis client of your choice. It is important that these commands
are executed in this exact order. 

    set     "#{prefix}-event-#{event_id}"   event_data
    lpush   "#{prefix}-queue"               event_id
    expire  "#{prefix}-event-#{event_id}"   event_ttl

This example assumes you have `redis_prefix` set to the default value 
("fnordmetric"). You have to choose a uniqe event id yourself

_Example: push event 123123 to the internal redis queue with a ttl of 10 minutes__

    set     "fnotmetric-event-123123"   "{ \"_type\": \"test\" }"
    lpush   "fnordmetric-queue"         "123"
    expire  "fnordmetric-event-123123"  "600"



### AMQP

here be dragons


### STOMP

here be dragons


### Client Libraries

There are a number of client libraries for FnordMetric for Ruby, PHP, Pythin, C# and
more. [List of available clients](/documentation/classic_examples)

