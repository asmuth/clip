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

