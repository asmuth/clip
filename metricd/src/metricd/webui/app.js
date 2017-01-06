/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

this["FnordMetric"] = (function() {
  'use strict';

  var app = this;
  var current_path;
  var router;
  var viewport_elem;
  var widgets = {};
  var current_view;

  var init = function() {
    console.log(">> FnordMetric v0.10");

    document.querySelector(".navbar").style.display = "block";
    showLoader();

    viewport_elem = document.getElementById("fm_viewport");
    setPath(window.location.pathname + window.location.search);

    /* handle history entry change */
    setTimeout(function() {
      window.addEventListener('popstate', function(e) {
        e.preventDefault();
        if (e.state && e.state.path) {
          applyNavgiationChange(e.state.path);
        } else {
          setPath(window.location.pathname + window.location.search);
        }
      }, false);
    }, 0);
  }


  var navigateTo = function(url) {
    var path = URLUtil.getPathAndQuery(url);
    history.pushState({path: path}, "", path);
    setPath(path);
  }

  var navigateHome = function() {
    navigateTo("/");
  }

  var showLoader = function() {
    //FIXME
    //document.getElementById("fm_main_loader").classList.remove("hidden");
  };

  var hideLoader = function() {
    //FIXME
    //document.getElementById("fm_main_loader").classList.add("hidden");
  };

  /** PRIVATE **/
  function setPath(path) {
    if (path == current_path) {
      return;
    }

    current_path = path;
    console.log(">> Navigate to: " + path);

    var params = {};
    params.app = app;
    params.path = path;

    /* try changing the path in the current view instance */
    if (current_view && current_view.changePath) {
      var route = findRoute(params.path);
      if (current_view.changePath(path, route)) {
        return;
      }
    }

    /* otherwise search for the new view class */
    showLoader();

    var route = params.path ? findRoute(params.path) : null;
    params.route = route;

    var view = route ? FnordMetric.views[route.view] : null;
    if (!view) {
      view = FnordMetric.views["fnordmetric.error"];
    }

    /* destroy the current view */
    if (current_view && current_view.destroy) {
      current_view.destroy();
    }

    /* initialize the new view */
    viewport_elem.innerHTML = "";
    current_view = {};

    view.call(current_view, viewport_elem, params);

    if (current_view.initialize) {
      current_view.initialize.call(current_view);
    }

    hideLoader();
  }

  function findRoute(full_path) {
    var path = full_path;
    var end = path.indexOf("?");
    if (end >= 0) {
      path = path.substring(0, end)
    }

    var routes = FnordMetric.routes;
    for (var i = 0; i < routes.length; i++) {
      if (routes[i].route instanceof RegExp) {
        var match = path.match(routes[i].route);
        if (match) {
          var route = {};
          for (var k in routes[i]) {
            route[k] = routes[i][k];
          }

          route.args = {}
          for (var j = 1; j < match.length; ++j) {
            route.args[route.route_args[j - 1]] = match[j];
          }

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

  this["init"] = init;
  this.navigateTo = navigateTo;
  this.navigateHome = navigateHome;
  this.showLoader = showLoader;
  this.hideLoader = hideLoader;
  this.views = {};
  this.util = {};
  this.api_base_path = "/api/v1/metrics";
  return this;
}).apply(this["FnordMetric"] || {});


