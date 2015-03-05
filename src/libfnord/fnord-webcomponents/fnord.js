/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
Fnord = {
  components: {}
};

Fnord.getTemplate = function(component, template) {
  var template_selector = "#" + component + "-" + template + "-tpl";

  var template = document.querySelector(template_selector);
  if (!template) {
    var template_import = document.querySelector(
        "link[data-component='" + component + "']");

    if (!template_import) {
      return null;
    }

    template = template_import.import.querySelector(template_selector);
  }

  if (!template) {
    return null;
  }

  return document.importNode(template.content, true);
};

Fnord.registerComponent = function(component, cb) {
  var proto = Object.create(HTMLElement.prototype);
  cb.apply(proto);
  Fnord.components[component] = proto;
  document.registerElement(component, { prototype: proto });
  return proto;
};

Fnord.ready = function() {
  window.dispatchEvent(new Event("fn-ready"));
  document.body.removeAttribute("data-unresolved");
};

Fnord.onWindowResize = function() {
  var items = document.body.querySelectorAll(".resizable");

  for (var i = 0; i < items.length; i++) {
    try {
      items[i].onResize();
    } catch (e) {
      console.log("function doesn't exists");
    }
  }
}

Fnord.httpGet = function(url, callback) {
  var http = new XMLHttpRequest();
  http.open("GET", url, true);
  http.send();

  var base = this;
  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      callback(http);
    }
  }
};

Fnord.httpPost = function(url, request, callback) {
  var http = new XMLHttpRequest();
  http.open("POST", url, true);
  var start = (new Date()).getTime();
  http.send(request);

  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      var end = (new Date()).getTime();
      var duration = end - start;
      callback(http, duration);
    }
  }
};

Fnord.jsonRPC = function(url, method, params, callback) {
  var req = {
    "jsonrpc": "2.0",
    "method": method,
    "params": params,
    "id": 0
  };

  var http = new XMLHttpRequest();
  http.open("POST", url, true);
  var start = (new Date()).getTime();
  http.send(JSON.stringify(req));

  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      var end = (new Date()).getTime();
      var duration = end - start;

      if (http.status != 200) {
        console.log("RPC failed", http.responseText);
        return;
      }

      var resp = JSON.parse(http.responseText);
      if (resp.error) {
        console.log("RPC failed", resp.error);
        return;
      }

      callback(resp.result);
    }
  }
};

Fnord.setAttributes = function(attrs, elem) {
  for (var key in attrs) {
    elem.setAttribute(key, attrs[key]);
  }
}

Fnord.appendLeadingZero = function (num) {
  var num = num;
  if (typeof num == 'string') {
    return (num.length > 1)? num : "0" + num;
  }
  return (num > 9)? num : "0" + num;
};

/**
  * @param offset in seconds
  */
Fnord.parseTimeOffset = function(offset) {
  if (offset < 60) {
    var label = (offset == 1)? " second ago" : " seconds ago";
    return offset + label;
  } else if (offset < 3600) {
    var time = Math.floor(offset / 60);
    var label = (time == 1)? " minute ago" : " minutes ago";
    return time + label;
  } else if (offset < 86400) {
    var time =  Math.floor(offset / 3600);
    var label = (time == 1)? " hour ago" : " hours ago";
    return time + label;
  } else {
    var time = Math.floor(offset / 86400);
    var label = (time == 1)? " day ago" : " days ago";
    return time + label;
  }
}

/**
  * creates a time description like 
  * '2 hours ago - Nov 8 2014 11:33:11
  * @param timestamp unix ts in seconds, milli or microseconds
  */
Fnord.parseTimestamp = function(timestamp) {
  if (timestamp == 0) {
    return "0";
  }

  var months = [
    "Jan","Feb", "Mar","Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct","Nov", "Dec"];

  var timestamp = Math.floor(timestamp / 1000);

  var now = Date.now();
  var date = new Date(timestamp);

  var offset =  Math.floor(
    (now - timestamp) / 1000);

  var time_str = Fnord.parseTimeOffset(offset);


  var minutes = date.getMinutes();
  if (minutes < 10) {
    minutes = "0" + minutes;
  }

  var seconds = date.getSeconds();
  if (seconds < 10) {
    seconds = "0" + seconds;
  }

  time_str +=
    " - " + 
    months[date.getMonth()] + 
    " " + date.getDate() +
    " " + date.getFullYear() +
    " " + date.getHours() +
    ":" + minutes +
    ":" + seconds

  return time_str;
}


Fnord.ready();
window.onresize = function() {
  Fnord.onWindowResize();
}
