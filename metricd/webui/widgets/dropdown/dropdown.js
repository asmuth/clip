/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
var DropdownComponent = function() {
  this.createdCallback = function() {
    var _this = this;
    this.querySelector("button.btn").addEventListener("click", function(e) {
      e.preventDefault();
      _this.showMenu();
    }, false);

    this.initMenuItems();
  }

  this.initMenuItems = function() {
    var _this = this;
    var items = this.querySelectorAll("ul li");
    for (var i = 0; i < items.length; i++) {
      items[i].addEventListener("click", function(e) {
        e.preventDefault();
        _this.onSelect(this);
      }, false);
    }
  }

  this.setValue = function(value) {
    var selected_item = this.querySelector("ul li[data-value='" + value + "']");
    if (selected_item) {
      this.selectItem(selected_item)
    }
  }

  this.showMenu = function() {
    this.classList.add("active");
  }

  this.hideMenu = function() {
    this.classList.remove("active");
  }

  this.onSelect = function(elem) {
    this.fireSelectEvent(elem);
    this.selectItem(elem);
    this.hideMenu();
  }

  this.selectItem = function(elem) {
    var current_selection = this.querySelector("ul li.selected");
    if (current_selection) {
      current_selection.classList.remove("selected");
    }

    elem.classList.add("selected");

    this.renderHeader(elem.textContent);
  }

  this.renderHeader = function(value) {
    this.querySelector("button.btn .value").innerHTML = DomUtil.escapeHTML(
        value);
  }

  this.fireSelectEvent = function(elem) {
    var ev = new CustomEvent(
      "select", {
        detail: {
          item: elem,
          value: elem.getAttribute("data-value"),
        },
        bubbles: true,
    });
    this.dispatchEvent(ev);
  }

}

var proto = Object.create(HTMLElement.prototype);
DropdownComponent.apply(proto);
document.registerElement("f-dropdown", { prototype: proto });

