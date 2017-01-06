/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

DOMUtil = this.DOMUtil || {};

DOMUtil.replaceContent = function(elem, new_content) {
  elem.innerHTML = "";
  elem.appendChild(new_content);
}

DOMUtil.clearChildren = function(elem) {
  elem.innerHTML = "";
}

/**
  * Set a cookie value
  * @param name the cookies name
  * @param value the cookies value
  * @param expires (optional) expiry date in GMTString format
  * (as returned by Date.toUTCString)
  */
DOMUtil.setCookie = function(name, value, expires) {
  var cookie_str = name + "=" + value;

  if (expires) {
    cookie_str += "; expires=" + expires;
  }

  document.cookie = cookie_str;
}

DOMUtil.getCookie = function(name) {
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

DOMUtil.onClick = function(elem, fn) {
  elem.addEventListener("click", function(e) {
    e.preventDefault();
    e.stopPropagation();
    fn.call(this, e);
    return false;
  });
};

DOMUtil.onEnter = function(elem, fn) {
  elem.addEventListener("keydown", function(e) {
    if (e.keyCode == 13) { //ENTER
      e.preventDefault();
      e.stopPropagation();
      fn.call(this, e);
      return false;
    }
  });
}

DOMUtil.escapeHTML = function(str) {
  if (str == undefined || str == null || str.length == 0) {
    return "";
  }
  var div = document.createElement('div');
  div.appendChild(document.createTextNode(str));
  return div.innerHTML;
};

