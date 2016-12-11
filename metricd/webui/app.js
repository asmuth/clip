/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

this["FnordMetric"] = (function() {
  'use strict'
  var app = this;
  var current_path = window.location.pathname + window.location.search;
  var router;
  var viewport_elem;
  var widgets = {};
  var current_view;

  var init = function() {
    console.log(">> FnordMetric v0.10");

    document.querySelector(".headbar").style.display = "block";
    showLoader();

    viewport_elem = document.getElementById("fm_viewport");
    setRoute(current_path);

    /* handle history entry change */
    setTimeout(function() {
      window.addEventListener('popstate', function(e) {
        e.preventDefault();
        if (e.state && e.state.path) {
          applyNavgiationChange(e.state.path);
        } else {
          applyNavigationChange(window.location.pathname + window.location.search);
        }
      }, false);
    }, 0);
  }

  function findRoute(full_path) {
    var path = full_path;
    var end = path.indexOf("?");
    if (end >= 0) {
      path = path.substring(0, end)
    }

    // PATH: /metrics/
    if (path == "/metrics") {
      return {
        "route": "/metrics",
        "view": "fnordmetric.metric.list",
      }
    }

    var m = path.match(new RegExp("^\/metrics\/(.*)$"));
    if (m) {
      return {
        "route": "/metrics",
        "view": "fnordmetric.metric.series.list",
        "args": m
      }
    }

    return null;
  }

  function setView(view, params) {
    if (current_view && current_view.destroy) {
      current_view.destroy();
    }

    viewport_elem.innerHTML = "";
    current_view = {};

    view.call(current_view, viewport_elem, params);

    if (current_view.initialize) {
      current_view.initialize.call(current_view);
    }
  }

  var navigateTo = function(url) {
    var path = URLUtil.getPathAndQuery(url);
    history.pushState({path: path}, "", path);
    applyNavigationChange(path);
  }

  var applyNavigationChange = function(path) {
    if (path == current_path) {
      return;
    }

    current_path = path;
    setRoute(path);
  };

  var navigateHome = function() {
    navigateTo("/");
  }

  var setRoute = function(path) {
    console.log(">> Navigate to: " + path);

    var params = {};
    params.app = app;
    params.path = path;

    //FIXME
    var m = path.match(/\/a\/([a-z0-9A-Z_-]+)(\/?.*)/);
    if (m) {
      params.project_id = m[1];
      params.vpath = "/a/<namespace>" + m[2];
    } else {
      params.vpath = path;
    }

    if (current_view && current_view.changePath) {
      var route = findRoute(params.vpath);
      if (current_view.changePath(path, route)) {
        return;
      }
    }

    /* render page */
    loadPage(params);
  }

  var loadPage = function(params) {
    showLoader();
    renderPage(params);
  }

  var renderPage = function(params) {
    var route = params.vpath ? findRoute(params.vpath) : null;
    params.route = route;

    var view = route ? FnordMetric.views[route.view] : null;
    if (!view) {
      renderError({type: "404"});
      return;
    }

    hideLoader();
    setView(view, params);
    current_view = view;
  }

  var showLoader = function() {
    //FIXME
    //document.getElementById("fm_main_loader").classList.remove("hidden");
  };

  var hideLoader = function() {
    //FIXME
    //document.getElementById("fm_main_loader").classList.add("hidden");
  };

  var renderError = function(cfg) {
    var view = FnordMetric.views["fnordmetric.error"];
    setView(view, cfg);
  }

  this["init"] = init;
  this.navigateTo = navigateTo;
  this.navigateHome = navigateHome;
  this.showLoader = showLoader;
  this.hideLoader = hideLoader;
  this.renderError = renderError;
  this.views = {};
  this.util = {};
  this.api_base_path = "/api/v1/metrics";
  return this;
}).apply(this["FnordMetric"] || {});


FnordMetric.util.rewriteLinks = function(elem, project_id) {
  var elems = elem.querySelectorAll("a");
  for (var i = 0; i < elems.length; ++i) {
    elems[i].href = elems[i].href.replace("%project_id%", project_id);
  }
};

FnordMetric.util.rewriteAndHandleLinks = function(elem, project_id) {
  FnordMetric.util.rewriteLinks(elem, project_id);
  DomUtil.handleLinks(elem, FnordMetric.navigateTo);
}

FnordMetric.util.renderBreadcrumbs = function(elem, crumbs) {
  DomUtil.clearChildren(elem);

  crumbs.forEach(function(c) {
    var crumb_elem = document.createElement("fm-breadcrumbs-section");
    var crumb_a = document.createElement("a");
    crumb_a.innerHTML = DomUtil.escapeHTML(c.title);
    if (c.href) {
      crumb_a.setAttribute("href", c.href);
    }

    crumb_elem.appendChild(crumb_a);
    elem.appendChild(crumb_elem);
  });
}
