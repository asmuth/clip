

+ widget_key is an arbitrary value chosen by the client and
  included in the response (used for pipelining)



Timeseries Data
===============

  request:

    {
      "namespace":"loveos",
      "type":"widget_request",
      "klass":"generic",
      "cmd":"values_at",
      "gauges":["user_logins", "user_logins_failed"],
      "since":1360317470,
      "until":1360333470,
      "widget_key": 123123123123,
    }

  response:

    {
      "namespace": "loveos",
      "type": "widget_response",
      "cmd": "values_at",
      "gauges": [
        {
          "key": "useronline_total",
          "title": "Useronline (total)",
          "vals": {
            1360326670: 123,
            1360326680: 123,
            1360326690: 123
          }
        }
      ],
      "widget_key": 123123123123,
    }


Single Value
============

  request:

    {
      "namespace": "loveos",
      "type": "widget_request",
      "klass": "generic",
      "cmd": "values_for",
      "gauge": "order_sum",
      "at": "-5h",
      "widget_key": 123123123
    }

  response:

    {
      "namespace":"loveos",
      "type":"widget_response",
      "cmd": "values_for",
      "gauge": "order_sum",
      "at": "-5h",
      "value": 123,
      "widget_key": 123123123
    }


