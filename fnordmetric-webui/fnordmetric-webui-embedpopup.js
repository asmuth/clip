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
  var popup;
  var background;

  function close() {
    if (popup.parentNode == elem) {
      elem.removeChild(popup);
    }
    if (background.parentNode == elem) {
      elem.removeChild(background);
    }
  }

  function render() {
    background = document.createElement("div");
    background.className = "popup_ui background";

    popup = document.createElement("div");
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

    var inner_window = document.createElement("div");

    elem.appendChild(background);
    popup.appendChild(close_btn);
    tabbar.appendChild(iframe_tab);
    tabbar.appendChild(url_tab);
    tabbar.appendChild(html_tab);
    popup.appendChild(tabbar);
    popup.appendChild(inner_window);
    elem.appendChild(popup);


    background.addEventListener('click', function(e) {
      e.preventDefault();
      close();
    }, false);

    close_btn.addEventListener('click', function(e) {
      e.preventDefault();
      close();
    }, false);

    iframe_tab.addEventListener('click', function(e) {
      e.preventDefault();
      inner_window.innerHTML = "FnordMetric IFrame";
    }, false);

    url_tab.addEventListener('click', function(e) {
      e.preventDefault();
      inner_window.innerHTML = "FnordMetric URL";
    }, false);

    html_tab.addEventListener('click', function(e) {
      e.preventDefault();
      inner_window.innerHTML = "FnordMetric HTML";
    }, false);


  }


  return {
    "render" : render,
  }
}

/* function closePopup() {
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
*/
