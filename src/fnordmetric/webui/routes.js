this["FnordMetric"] = this["FnordMetric"] || {}

FnordMetric.routes = [
  {
    "route": "/ui/metrics",
    "view": "fnordmetric.metric.list",
  },
  {
    "route": new RegExp("^/ui/metrics/(.*)$"),
    "route_args": [ "metric_id" ],
    "view": "fnordmetric.metric.detail",
  }
];
