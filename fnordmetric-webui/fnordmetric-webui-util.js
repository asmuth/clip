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

/**
 *
 * TODOS:
 *
 * Query Playground:
 *  - "embed this query" opens up a popup with html/js/ruby snippets
 *  - prevent reload/navigation to other page (body onunload)
 *  - stretch: display a [fake] loading bar
 *  - nice to have: represent current chart and table, view in url --> renderResultPane
 *
 * Metric list view:
 *  - write meaningful error messages
 *
 *  - fix menuitems
 *  - fix resize tooltip, vertical split
 *  - wrong URL: redirect and display message 
 *  - fix back and for (push and pop states)
 *
 */


if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.util === undefined) {
  FnordMetric.util = {};
}

FnordMetric.util.parseQueryString = function(qstr) {
  var path;
  var query_params = {};

  if (qstr.indexOf("?") >= 0) {
    path = qstr.substr(0, qstr.indexOf("?"))

    var params_str = qstr.substr(qstr.indexOf("?") + 1);
    var raw_params = params_str.split('&');
    for (var i in raw_params) {
      var param = raw_params[i].split('=');
      query_params[decodeURIComponent(param[0])] = decodeURIComponent(param[1]);
    }
  } else {
    path = qstr;
  }

  return {
    "path": path,
    "query_params": query_params
  };
}

FnordMetric.util.displayLoader = function(elem) {
  elem.innerHTML = "<div class='load_foreground'></div>";
}

FnordMetric.util.displayErrorMessage = function(elem, msg) {
  elem.innerHTML = "<div>" + msg + "</div>"; // XSS!
}


FnordMetric.httpGet = function(url, callback) {
  var http = new XMLHttpRequest();
  http.open("GET", url, true);
  http.send();

  http.onreadystatechange = function() {
    if (http.readyState == 4) {
      callback(http);
    }
  }
}

FnordMetric.httpPost = function(url, request, callback) {
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
}









FnordMetric.createButton = function(href, class_name, inner_HTML) {
  var button = document.createElement("a");
  button.href = "#";
  if (class_name !== undefined) {
    button.className = class_name;
  }
  if (inner_HTML !== undefined) {
    button.innerHTML = inner_HTML;
  }
  return button;
}

FnordMetric.DropdownAutocomplete = function(
  parentNode, dropdown, input_field, keys, search_button) {
  var down = 0;
  var current_value = "";
  var dropdownKeyNav = function() {
    var dropdown_items = dropdown.childNodes;
    var i = down -1;
    if (i < dropdown_items.length) {
      if (i > 0) {
        dropdown_items[i - 1].className = "";
      }
      if (i+1 < dropdown_items.length) {
        dropdown_items[i+1].className = "";
      }
      current_value = dropdown_items[i].firstChild.innerHTML;
      dropdown_items[i].className = "hover";
    }
  }

  var destroyDropdown = function() {
    down = 0;
    current_value = "";
    while (dropdown.firstChild) {
      dropdown.removeChild(dropdown.firstChild);
    }
  }



  var autocomplete = function(input) {
    destroyDropdown();
    parentNode.appendChild(dropdown);

    keys.map(function(key) {
      if (key.indexOf(input) > - 1) {
        var dropdown_item = document.createElement("li");
        var dropdown_link = FnordMetric.createButton(
          "#", undefined, key);
        dropdown_item.appendChild(dropdown_link);
        dropdown.appendChild(dropdown_item);

        dropdown_link.addEventListener('click', function(e) {
          e.preventDefault();
          input_field.value = this.innerHTML;
          destroyDropdown();
        }, false);
      }
    });

  }

  var init = function() {
    input_field.addEventListener('focus', function(e) {
      this.value = "";
    }, false);

    input_field.addEventListener('input', function(e) {
        autocomplete(this.value)
    }, false);

    input_field.addEventListener('keydown', function(e) {
      switch (e.keyCode) {
        case 13:
          e.preventDefault();
          if (current_value.length > 0) {
            input_field.value = current_value;
          } else {
            FnordMetric.WebUI().renderSearchResult(input_field.value);
          }
          destroyDropdown();
          break;
        case 40:
          down++;
          dropdownKeyNav();
          break;
        case 38:
          down--;
          dropdownKeyNav();
          break;
        default:
          break;
      }
    }, false);
  }
  init();
}


