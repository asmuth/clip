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
if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.views === undefined) {
  FnordMetric.views = {};
}


FnordMetric.util.embedPopup = function(elem) {
  function render() {
    console.log("render popup");
    var background = document.createElement("div");
    background.className = "popup_ui background";
    elem.appendChild(background);

    var popup = document.createElement("div");
    popup.className = "popup_ui";

    var tabbar = document.createElement("div");
    tabbar.className = "controls";

    var close_btn = FnordMetric.createButton(
      "#", "close_btn", "X");

    var iframe_tab = FnordMetric.createButton(
      "#", "tab", "iFrame");

    var url_tab = FnordMetric.createButton(
      "#", "tab", "URL");

    var html_tab = FnordMetric.createButton(
      "#", "tab", "HTML");

    popup.appendChild(close_btn);
    tabbar.appendChild(iframe_tab);
    tabbar.appendChild(url_tab);
    tabbar.appendChild(html_tab);
    popup.appendChild(tabbar);
    elem.appendChild(popup);
  }

  function close() {

  }

  return {
    "render" : render,
  }
}
