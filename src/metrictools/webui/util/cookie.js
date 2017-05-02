/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

CookieUtil = this.CookieUtil || {};

/**
  * Set a cookie value
  * @param name the cookies name
  * @param value the cookies value
  * @param expires (optional) expiry date in GMTString format
  * (as returned by Date.toUTCString)
  */
CookieUtil.set = function(name, value, expires) {
  var cookie_str = name + "=" + value;

  if (expires) {
    cookie_str += "; expires=" + expires;
  }

  document.cookie = cookie_str;
}

CookieUtil.get = function(name) {
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

