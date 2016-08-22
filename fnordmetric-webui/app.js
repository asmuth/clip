/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *   Copyright (c) 2016 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

this["FnordMetric"] = (function() {
  var app = this;
  var current_project;
  var user_info = {};
  var router;
  var navigator;
  var viewport;
  var widgets = {};
  var api_stubs = {};
  var credentials = {};

  var init = function() {
    console.log(">> FnordMetric v0.10");
    loadConfig();

    router = new zRouter(app.config.routes);
    navigator = new zNavigator(window);
    viewport = new zViewport(document.getElementById("fm_viewport"));

    widgets.main_menu = new FnordMetric.MainMenu(document.getElementById("fm_main_menu"));
    widgets.navbar = new FnordMetric.Navbar(document.querySelector(".fm_navbar"));

    navigator.onNavigationChange(setRoute);
    showLoader();

    api_stubs.auth = new AuthAPI(app.config.api_hosts.auth);
    api_stubs.auth.getCredentials({}, function(resp) {
      //FIXME
      resp.success = true;
      resp.result = {credentials: {}};
      if (resp.success) {
        credentials = resp.result.credentials;
        setupAPIStubs();
        //FIXME
        //fetchUserInfo(function() {
          setRoute(navigator.getPath());
        //});
      } else {
        navigateToAuthPage();
      }
    });
  }

  var loadConfig = function() {
    var cfg = JSON.parse(document.querySelector("#fnordmetric_config").textContent);
    app.config = app.config || {};
    for (k in cfg) {
      app.config[k] = cfg[k];
    }
  }

  var navigateTo = function(url) {
    navigator.navigateTo(zURLUtil.getPathAndQuery(url));
  }

  var navigateHome = function() {
    if (current_project) {
      navigator.navigateTo("/a/" + current_project);
    } else {
      navigator.navigateTo("/a/");
    }
  }

  var navigateToAuthPage = function() {
    document.location.href = app.config.login_url;
  }

  var setRoute = function(path) {
    console.log(">> Navigate to: " + path);

    var params = {};
    params.app = app;
    params.path = path;
    params.api_stubs = api_stubs;
    params.user_info = user_info;

    var m = path.match(/\/a\/([a-z0-9A-Z_-]+)(\/?.*)/);
    if (m) {
      params.project_id = m[1];
      params.vpath = "/a/<namespace>" + m[2];
    } else {
      params.vpath = path;
    }

    var current_view = viewport.getView();
    if (current_view && current_view.changePath) {
      var route = router.findRoute(params.vpath);
      if (current_view.changePath(path, route)) {
        return;
      }
    }

    /* render page */
    loadPage(params);
  }

  var loadPage = function(params) {
    showLoader();
    if (params.project_id != current_project) {
      renderPage(params);
    } else {
      widgets.navbar.setProjectID(current_project);
      renderPage(params);
    }
  }

  var renderPage = function(params) {
    var route = params.vpath ? router.findRoute(params.vpath) : null;
    params.route = route;

    var view = route ? FnordMetric.views[route.view] : null;
    if (!view) {
      //FIXME render 404
    }

    if (route && route.main_menu) {
      widgets.main_menu.setActiveItem(route.main_menu_active_item);
      widgets.main_menu.showMenu();
    } else {
      widgets.main_menu.hideMenu();
    }

    hideLoader();
    viewport.setView(view, params);
  }

  var setupAPIStubs = function() {
    api_stubs.user = new UserAPI(app.config.api_hosts.api_user, credentials.api_token);
  }

  var showLoader = function() {
    document.getElementById("fm_main_loader").classList.remove("hidden");
  };

  var hideLoader = function() {
    document.getElementById("fm_main_loader").classList.add("hidden");
  };

  this["init"] = init;
  this.navigateTo = navigateTo;
  this.navigateHome = navigateHome;
  this.showLoader = showLoader;
  this.hideLoader = hideLoader;
  this.views = {};
  this.util = {};
  this.api_stubs = api_stubs;
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
  zDomUtil.handleLinks(elem, FnordMetric.navigateTo);
}

FnordMetric.util.renderBreadcrumbs = function(elem, crumbs) {
  zDomUtil.clearChildren(elem);

  crumbs.forEach(function(c) {
    var crumb_elem = document.createElement("fm-breadcrumbs-section");
    var crumb_a = document.createElement("a");
    crumb_a.innerHTML = zDomUtil.escapeHTML(c.title);
    if (c.href) {
      crumb_a.setAttribute("href", c.href);
    }

    crumb_elem.appendChild(crumb_a);
    elem.appendChild(crumb_elem);
  });
}
