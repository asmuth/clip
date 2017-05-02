/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

var TabbableComponent = function() {
  this.setContent = function() {
    var active_content = this.querySelector("f-tab-content[data-active]");
    if (active_content) {
      active_content.removeAttribute('data-active');
    }

    var tab = getActiveTab.call(this);
    var index = tab.getAttribute("data-index");
    var tab_content =
      this.querySelector("f-tab-content[data-index='" + index + "']");
    if (tab_content) {
      tab_content.setAttribute('data-active', 'active');
    }
  };


  var setTabWidth = function() {
    var tabs = this.querySelectorAll("f-tab");
    if (tabs.length == 0) {
      return;
    }
    var width = 100 / tabs.length;
    for (var i = 0; i < tabs.length; i++) {
      tabs[i].style.width = width + "%";
    }
  };


  var onTabClick = function(tab) {
    var current_active_tab = this.querySelector("f-tab[data-active]");
    current_active_tab.removeAttribute("data-active");
    tab.setAttribute("data-active", "active");

    if (this.hasAttribute("data-content-attached")) {
      this.setContent(tab.getAttribute('data-index'));
    }

    var click_event = new CustomEvent("tab-click", {
      bubbles: true,
      cancelable: true});
    tab.dispatchEvent(click_event);
  };


  var getActiveTab = function() {
    var active_tab = this.querySelector("f-tab[data-active]");
    if (active_tab != null) {
      return active_tab;
    }

    // set first tab active
    active_tab = this.querySelector("f-tab");
    if (active_tab) {
      active_tab.setAttribute('data-active', 'active');
    }

    return active_tab;
  };


  var init = function() {
    if (this.hasAttribute('data-content-attached')) {
      this.setContent();
    }

    if (this.hasAttribute('data-evenly-sized')) {
      setTabWidth.call(this);
    }

    var tabs = this.querySelectorAll("f-tab");
    for (var i = 0; i < tabs.length; i++) {
      tabs[i].addEventListener('click', (function(_this) {
        return function(e) {
          onTabClick.call(_this, this);
        }
      })(this));
    }
  };


  this.createdCallback = function() {
    init.call(this);
  };

  this.attributeChangedCallback = function(attr, old_val, new_val) {
    if (attr == 'data-content-attached') {
      this.setContent();
    }
  };

};

var proto = Object.create(HTMLElement.prototype);
TabbableComponent.apply(proto);
document.registerElement("f-tabbable", { prototype: proto });
