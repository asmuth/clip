

+ widget_key is an arbitrary value chosen by the client and
  included in the response (used for pipelining)



Timeseries Data
===============

  request:

    {
      "namespace":"loveos",
      "type":"widget_request",
      "klass":"TimeseriesWidget",
      "cmd":"values_at",
      "gauges":["user_logins", "user_logins_failed"],
      "since":1360317470,
      "until":1360333470,
      "widget_key": 123123123123,
    }

  response:



Single Value
============

  request:

    {
      "namespace": "loveos",
      "type": "widget_request",
      "klass": "generic",
      "cmd": "values_for",
      "gauge": "order_sum",
      "offsets": ["s3600"],
      "widget_key": 123123123
    }

  response:

    {
      "namespace":"loveos",
      "type":"widget_response",
      "cmd": "values_for",
      "gauge": "order_sum",
      "values": {
        "order_sum-0-216000": {
          "value": 21964
        }
      },
      "widget_key": 123123123
    }


