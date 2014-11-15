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
    popup.className = "popup";

    var close_btn = FnordMetric.createButton(
      "#", undefined, "X");

    var innerWindow = document.createElement("div");
    innerWindow.className = "inner_window";

    popup.appendChild(close_btn);
    popup.appendChild(innerWindow);
    elem.appendChild(

  }

  function close() {

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
