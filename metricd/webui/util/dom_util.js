/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
DomUtil = this.DomUtil || {};

DomUtil.replaceContent = function(elem, new_content) {
  elem.innerHTML = "";
  elem.appendChild(new_content);
}

DomUtil.clearChildren = function(elem) {
  elem.innerHTML = "";
}

/**
  * Set a cookie value
  * @param name the cookies name
  * @param value the cookies value
  * @param expires (optional) expiry date in GMTString format
  * (as returned by Date.toUTCString)
  */
DomUtil.setCookie = function(name, value, expires) {
  var cookie_str = name + "=" + value;

  if (expires) {
    cookie_str += "; expires=" + expires;
  }

  document.cookie = cookie_str;
}

DomUtil.getCookie = function(name) {
  var cookie = "; " + document.cookie;
  var parts = cookie.split("; ");

  for (var i = 0; i < parts.length; i++) {
    var c = parts[i].split("=");
    if (c[0] == name) {
      return c[1];
    }
  }

  return null;
}

DomUtil.onClick = function(elem, fn) {
  elem.addEventListener("click", function(e) {
    e.preventDefault();
    e.stopPropagation();
    fn.call(this, e);
    return false;
  });
};

DomUtil.onEnter = function(elem, fn) {
  elem.addEventListener("keydown", function(e) {
    if (e.keyCode == 13) { //ENTER
      e.preventDefault();
      e.stopPropagation();
      fn.call(this, e);
      return false;
    }
  });
}

DomUtil.handleLinks = function(elem, fn) {
  var click_fn = (function() {
    return function(e) {
      fn(this.getAttribute("href"));
      e.preventDefault();
      e.stopPropagation();
      return false;
    };
  })();

  var elems = elem.querySelectorAll("a");
  for (var i = 0; i < elems.length; ++i) {
    if (elems[i].hasAttribute("href")) {
      elems[i].addEventListener("click", click_fn);
    }
  }
};

DomUtil.escapeHTML = function(str) {
  if (str == undefined || str == null || str.length == 0) {
    return "";
  }
  var div = document.createElement('div');
  div.appendChild(document.createTextNode(str));
  return div.innerHTML;
};

DomUtil.nl2br = function(str) {
  return str.replace(/\n/g, "<br />");
};

DomUtil.nl2p = function(str) {
  var lines = str.split("\n\n");

  return lines.map(function(s) {
    return "<p>" + s.replace(/\n/g, "<br />")  + "</p>";
  }).join("\n");
};

DomUtil.textareaGetCursor = function(elem) {
  if ("selectionStart" in elem && document.activeElement == elem) {
    return elem.selectionStart;
  }

  if (elem.createTextRange) {
    var s = document.section.createRange();
    s.moveStart ("character", -elem.value.length);
    return s.text.split("\n").join("").length;
  }

  return -1;
}

DomUtil.textareaSetCursor = function(elem, pos) {
  if ("selectionStart" in elem) {
    setTimeout(function() {
      elem.selectionStart = pos;
      elem.selectionEnd = pos;
    }, 1);
    return;
  }

  if (elem.createTextRange) {
    var rng = elem.createTextRange();
    rng.moveStart("character", pos);
    rng.collapse();
    rng.moveEnd("character", 0);
    rng.select();
    return;
  }
}

