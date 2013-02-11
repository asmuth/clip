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

here be dragons


### AMQP

here be dragons


### STOMP

here be dragons

