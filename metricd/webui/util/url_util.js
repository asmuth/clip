/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
window.URLUtil = (window.URLUtil || {});

URLUtil.getPathAndQuery = function(url) {
  var a = document.createElement('a');
  a.href = url;

  var path = a.pathname;
  if (a.search) {
    path += a.search;
  }

  return path;
}

URLUtil.splitFilepath = function(path) {
  var parts = [];

  if (!path.endsWith("/")) {
    path += "/";
  }

  var pos = 0;
  if (path.startsWith("/")) {
    pos++;
  }

  for (var i = pos; i < path.length; ++i) {
    if (path.charAt(i) == '/' && (i < 1 || path.charAt(i - 1) != '\\')) {
      parts.push(path
          .substring(pos, i)
          .replace(/\\\//g, "/")
          .replace(/\\\\/g, "\\"));
      pos = i + 1;
    }
  }

  return parts;
}

URLUtil.buildQueryString = function(params) {
  var qs = [];

  for (var key in params) {
    var value = params[key];
    qs.push(encodeURIComponent(key) + "=" + encodeURIComponent(value));
  }

  return qs.join("&");
};

URLUtil.getParamValue = function(url, key) {
  var a = document.createElement('a');
  a.href = url;

  var query_string = a.search.substr(1);
  var key = encodeURIComponent(key) + "=";
  var params = query_string.split("&");
  for (var i = 0; i < params.length; i++) {
    if (params[i].lastIndexOf(key, 0) > -1) {
      return decodeURIComponent(params[i].substr(key.length));
    }
  }

  return null;
};

URLUtil.addOrModifyParam = function(url, param, new_value) {
  var a = document.createElement('a');
  a.href = url;

  var vars = a.search.substr(1).split("&");
  var new_vars = "";
  var found = false;

  for (var i = 0; i < vars.length; i++) {
    if (vars[i].length == 0) {
      continue;
    }

    var pair = vars[i].split('=', 2);
    var val = pair[1];
    if (pair[0] == param) {
      val = new_value;
      found = true;
    }

    new_vars += pair[0] + "=" + val + "&";
  }

  if (!found) {
    new_vars += param + "=" + new_value;
  }

  a.search = "?" + new_vars;
  return a.href;
}

/* adds for each value param to the url e.g. device=mobile&device=desktop
*/
URLUtil.addOrModifyParamList = function(url, param, values) {
  var a = document.createElement('a');
  a.href = url;

  var vars = a.search.substr(1).split("&");
  var new_vars = [];

  for (var i = 0; i < vars.length; i++) {
    if (vars[i].length == 0) {
      continue;
    }

    var pair = vars[i].split('=', 2);
    if (pair[0] != param) {
      new_vars.push(vars[i]);
    }
  }

  values.forEach(function(val) {
    new_vars.push(param + "=" + val);
  });

  a.search = "?" + new_vars.join("&");
  return a.href;
};

URLUtil.getParamValueList = function(url, key) {
  var a = document.createElement('a');
  a.href = url;

  var values = [];

  var query_string = a.search.substr(1);
  var key = encodeURIComponent(key) + "=";
  var params = query_string.split("&");
  for (var i = 0; i < params.length; i++) {
    if (params[i].lastIndexOf(key, 0) > -1) {
      values.push(decodeURIComponent(params[i].substr(key.length)));
    }
  }

  return values;
}

URLUtil.getPath = function(url) {
  return url.split("?")[0]; // FIXME
};

URLUtil.getParams = function(url) {
  var res = {};

  var a = document.createElement('a');
  a.href = url;
  var query_string = a.search.substr(1);
  var params = query_string.split("&");
  for (var i = 0; i < params.length; i++) {
    var p = params[i].split("=");

    var key;
    var value;
    switch (p.length) {
      case 2:
        value = p[1];
        /* fallthrough */
      case 1:
        key = p[0];
        break;
      default:
        continue;
    }

    res[decodeURIComponent(key)] = decodeURIComponent(value);
  }

  return res;
}

URLUtil.comparePaths = function(a, b) {
  var a_path = URLUtil.getPath(a);
  var b_path = URLUtil.getPath(b);
  var a_params = URLUtil.getParams(a);
  var b_params = URLUtil.getParams(b);

  var params_changed =  {};
  var keys = Object.keys(a_params).concat(Object.keys(b_params));
  for (var i = 0; i < keys.length; i++) {
    if (keys[i].length == 0) {
      continue;
    }

    if (a_params[keys[i]] != b_params[keys[i]]) {
      params_changed[keys[i]] = true;
    }
  }

  return {
    path: a_path != b_path,
    params: Object.keys(params_changed)
  };
}

URLUtil.joinPaths = function(base_path) {
  var path_parts = [base_path];

  //further arguments are parts to add to the path to
  for (i = 1; i < arguments.length; i++) {
    path_parts.push(encodeURIComponent(arguments[i]));
  }

  return path_parts.join("/");
}

