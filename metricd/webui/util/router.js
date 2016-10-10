/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
var Router = function() {

  var routes = [
    {
      "route": new RegExp("/metrics/(.*)/(.*)"),
      "view": "fnordmetric.metric.sensor",
    },
    {
      "route": new RegExp("/metrics/(.*)"),
      "view": "fnordmetric.metric",
    },
    {
      "route": "/metrics",
      "view": "fnordmetric.metric.list",
    }
  ];

  this.findRoute = function(full_path) {
    var path = full_path;
    var end = path.indexOf("?");
    if (end >= 0) {
      path = path.substring(0, end);
    }

    for (var i = 0; i < routes.length; i++) {
      if (routes[i].route instanceof RegExp) {
        var match = path.match(routes[i].route);
        if (match) {
          var route = {};
          for (k in routes[i]) {
            route[k] = routes[i][k];
          }
          route.args = match;
          return route;
        }
      } else {
        if (path === routes[i].route) {
          return routes[i];
        }
      }
    }

    return null;
  }
}
