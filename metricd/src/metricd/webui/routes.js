this["FnordMetric"] = this["FnordMetric"] || {}

FnordMetric.routes = [
  {
    "route": "/metrics",
    "view": "fnordmetric.metric.list",
  },
  {
    "route": new RegExp("^\/metrics\/(.*)$"),
    "route_args": [ "metric_id" ],
    "view": "fnordmetric.metric.series.list",
  }
];
