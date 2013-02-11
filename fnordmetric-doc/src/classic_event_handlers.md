Event Handler
-------------

The basic unit of input data in FnordMetric is called an event. These events
are JSON objects (arbitrary hashmaps) A event may look like this:

    { "_type": "sale", "product_id": 534221, "purchase_value": 2999 }


Some events have a special format and meaning (like "increment this counter by 4"
or "add this sample to the average query time gauge"). You can just send these
special events and FnordMetric will know what do to with them.

_Example: increment something by four_

    ...


You can also send completely custom events and write a piece of ruby code that
processes them. In this case the only requirement is that the event has a
`_type` key, which is used to identify the correct event handler. All other fields
can be completely custom. This is useful if you want to perform actions on more
than one gauge but only want to send one piece of data from your app to FnordMetric.

_Example: send a "user signed up" event and increment two gauges_

    fnord...



There is a small number of keys which have a special meaning, all of them
are prefixed with an underscore:

    fixpaul: special key table
    - events containing user data (_session, _name)
    - _namespace

### Sessions

  here be dragons


### API Reference

      - data, time, session etc methods
      - incr, incr_field etc.
      - storing data in the session
      - end-of-session callback
