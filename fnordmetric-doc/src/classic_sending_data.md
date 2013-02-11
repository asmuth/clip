Sending Data
------------

FnordMetric operates on an input stream of "events". These events are JSON
objects (arbitrary hashmaps). A event may look like this:

    { "_type": "sale", "product_id": 534221, "purchase_value": 2999 }


There is a small number of keys which have a special meaning, all of them
are prefixed with an underscore:

    fixpaul: special key table
    - events containing user data (_session, _name)
    - _namespace


You can choose between a variety of ways to submit these events to FnordMetric:


### HTTP API

here be dragons


### TCP / UDP API

here be dragons


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

There are a number of client libraries for FnordMetric for Ruby, PHP, Pythin, C#,
etcetera. [List of clients](/documentation/examples)

