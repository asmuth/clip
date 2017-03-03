/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

var PopupComponent = function() {
  this.createdCallback = function() {
    var _this = this;
    this.onclick = function(e) {
      _this.close();
    };

    this.querySelector("f-popup-window").onclick = function(e) {
      e.stopPropagation();
    };
  };

  this.show = function() {
    this.setAttribute('data-active', 'active');

    var on_escape = function(e) {
      if (e.keyCode == 27) {
        this.close();
      }
    };

    this.escape_listener = on_escape.bind(this);
    document.addEventListener('keyup', this.escape_listener, false);
  };

  this.close = function() {
    this.removeAttribute("data-active");
    document.removeEventListener('keyup', this.escape_listener, false);
    var ev = new CustomEvent(
        "close",
        { bubbles: true, cancelable: true });

    this.dispatchEvent(ev);
  };

  this.isVisible = function() {
    return this.hasAttribute("data-active");
  }
}

var proto = Object.create(HTMLElement.prototype);
PopupComponent.apply(proto);
document.registerElement("f-popup", { prototype: proto });

