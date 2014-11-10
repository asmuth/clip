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
  var query_params;

  if (qstr.indexOf("?") >= 0) {
    path = qstr.substr(0, qstr.indexOf("?"))

    var params_str = qstr.substr(qstr.indexOf("?") + 1);
    var raw_params = params_str.split('&');
    for (var i in raw_params) {
      var param = raw_params[i].split('=');
      query_params = {};
      query_params.name = decodeURIComponent(param[0]);
      query_params.value = decodeURIComponent(param[1]);
    }
  } else {
    path = qstr;
  }

  return {
    "path": path,
    "query_params": query_params
  };
}

FnordMetric.util.convertArrayToString = function(array) {
  var string = "";
  if (array.length > 0) {
    string = array.join(", ");
  }
  return string;
}

/* simple loader foreground */
FnordMetric.util.displayLoader = function(elem) {
  elem.innerHTML = "<div class='load_foreground'><i class='fa fa-refresh fa-spin'></div>";
}

/*
 * loader foreground if loader can't be 
 * destroyed with resetting the innerHTML 
*/
FnordMetric.util.Loader = function() {
  var loader  = document.createElement("div");
  loader.className = "load_foreground";
  on_click = null;

  function onClick(on_click_new) {
    on_click = on_click_new;
  }

  function display(elem) {
    elem.appendChild(loader);
    if (on_click != null) {
      loader.onclick = on_click;
    }
  }

  function destroy(elem) {
    //FIXME
    loader = elem.querySelector(".load_foreground");
    elem.removeChild(loader);
  }

  return {
    "display" : display,
    "destroy" : destroy,
    "onClick" : onClick
  }
}


FnordMetric.util.displayErrorMessage = function(elem, msg) {
  elem.innerHTML = "<div>" + msg + "</div>"; // XSS!
}

FnordMetric.util.setURLQueryString = function(name, value, encode) {
  var fragment = window.location.hash.substr(1).split("?");
  fragment = fragment.length > 0 ? fragment[0] : "";
  var value = value;
  if (encode) {
    value = encodeURIComponent(value);
  }
  var hash = fragment + "?" + name + "=" + value;
  window.location.hash = hash;
}

FnordMetric.util.setFragmentURL = function(hash, name, value, encode, push_state) {
  var path = window.location.pathname;
  var value = value;
  if (encode == true) {
    value = encodeURIComponent(value);
  }
  var hash = 
    path + "#" + hash + "?" + name + "=" + value;
  window.location = hash;
  console.log("push state: " + push_state);
  if (push_state == true) {
    window.history.pushState({url: hash}, "#", hash);
  }
}

FnordMetric.util.openPopup = function(elem, text) {
  function closePopup() {
    elem.removeChild(popup);
    loader.destroy(elem);
  }

  var loader = FnordMetric.util.Loader();
  loader.onClick(closePopup);
  loader.display(elem);
  var popup = document.createElement("div");
  popup.className = "popup";
  var close_btn = FnordMetric.createButton(
    "#", undefined, "X");
  var innerWindow = document.createElement("div");
  innerWindow.className = "inner_window";
  innerWindow.innerHTML = text;

  popup.appendChild(close_btn);
  popup.appendChild(innerWindow);
  elem.appendChild(popup);

  close_btn.onclick = function(e) {
    e.preventDefault();
    closePopup();
  }
}

FnordMetric.util.renderPageHeader = function(text, elem) {
  var header = document.createElement("h1");
  header.className = "page_header";
  header.innerHTML = text;

  elem.appendChild(header);
}

FnordMetric.util.parseTimestamp = function(timestamp) {
  if (timestamp == 0) {
    return "0";
  }

  var time_str;
  var timestamp = timestamp / 1000;
  var now = Date.now();
  var date = new Date(timestamp);

  var offset =  Math.floor(
    (now - timestamp) / 1000);
  if (offset < 60) {
    var label = (offset == 1)? " second ago" : " seconds ago";
    time_str  = offset + label;
  } else if (offset < 3600) {
    var time = Math.floor(offset / 60);
    var label = (time == 1)? " minute ago" : " minutes ago";
    time_str = time + label;
  } else if (offset < 86400) {
    var time =  Math.floor(offset / 3600);
    var label = (time == 1)? " hour ago" : " hours ago";
    time_str = time + label;
  } else {
    var time = Math.floor(offset / 86400);
    var label = (time == 1)? " day ago" : " days ago";
    time_str = time + label;
  }

  var months = ["Jan", "Feb", "Mar", "Apr", "May",
    "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]

  var minutes = date.getMinutes();
  if (minutes < 10) {
    minutes = "0" + minutes;
  }

  var seconds = date.getSeconds();
  if (seconds < 10) {
    seconds = "0" + seconds;
  }

  time_str +=
    " - " + months[date.getMonth()] + 
    " " + date.getDate() +
    " " + date.getFullYear() +
    " " + date.getHours() +
    ":" + minutes +
    ":" + seconds

  return time_str;
}

FnordMetric.util.parseMilliTS = function(ts) {
  if (ts < 1000) {
    if (ts == 0) {
      return " less than 1 millisecond";
    } else if (ts == 1) {
      return " 1 millisecond";
    } else {
      return ts + " milliseconds";
    }
  } else if (ts < 60000) {
    ts = ts / 1000;
  return (ts + (ts == 1? " second" : " seconds"));
  } else {
    ts = ts / 60000;
    return (ts + (ts == 1? " minute" : " minutes"));
  }
}

FnordMetric.util.humanCountRows = function(tables) {
  var num = 0;
  tables.map(function(table) {
    num += table.rows.length;
  });
  return (num == 1? num + " row" : num + " rows")
}


FnordMetric.util.sortMetricList = function(metrics, column_index, order) {
  function compare(a, b) {
    if (a < b) {
      if (order == "asc") {
        return -1;
      } else {
        return 1;
      }
    }
    if (a > b) {
      if (order == "asc") {
        return 1;
      } else {
        return -1;
      }
    }
    return 0;
  }

  var sorted_metrics = metrics;
  column_index = parseInt(column_index);
  switch (column_index) {
    case 0:
      sorted_metrics.sort(function(a, b) {
        return (compare(
          a[column_index].toLowerCase(),
          b[column_index].toLowerCase()));
      });
      break;
    case 2:
      sorted_metrics.sort(function(a, b) {
        return (compare(
          a[column_index].toLowerCase(), 
          b[column_index].toLowerCase()));
      });
      break;
    case 3:
      sorted_metrics.sort(function(a, b) {
        return (compare(
          a[column_index], b[column_index]));
      });
      break;
    default:
      break;
  }
}

FnordMetric.util.getHorizontalEditorHeight = function(
  editor_height, result_height) {
    var default_height = (window.innerHeight - 49);
    editor_height = Math.max(editor_height, default_height);
    var height = Math.max(editor_height, result_height);
    return height;
}

FnordMetric.util.getHorizontalEditorWidth = 
  function(editor_width) {
    //returns the percental editor width
    var wdn_width = window.innerWidth;
    if (editor_width > 0) {
      editor_width = wdn_width / editor_width;
    }
    var width = Math.max(35, Math.min(50, editor_width));
    return width;
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

FnordMetric.util.searchMetricList = function(metrics, search_item) {
  //FIXME works but seems not to be the best solution
  var data = [];
  metrics.map(function(item) {
   if (item.key.indexOf(search_item) > -1) {
      data.push(item);
    }
  });
  return data;
}

FnordMetric.util.htmlEscape = function(str) {
  return str;
}

FnordMetric.util.insertAfter = function(elem, ref_elem) {
  ref_elem.parentNode.insertBefore(elem, ref_elem.nextSibling);
}



FnordMetric.util.Autocomplete = function(input, source) {
  console.log("autocomplte");
  var list = document.createElement("ul");
  list.className = "autocomplete";
  source.map(function(item) {
    var li = document.createElement("li");
    li.innerHTML = item;
    list.appendChild(li);
  });

  FnordMetric.util.insertAfter(list, input);

  input.addEventListener('keydown', function(e) {
    console.log("keydown in input");
    console.log(input.value);
  }, false);

}


/*

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

*/
