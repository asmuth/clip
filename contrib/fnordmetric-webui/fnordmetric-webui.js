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
if (typeof FnordMetric == "undefined") {
  FnordMetric = {};
}

FnordMetric.WebUI = function() {
  var navbar = document.createElement("div");
  navbar.id = "navbar";
  document.body.appendChild(navbar);

  var renderQueryEditor = function() {
    var editor = document.createElement("div");
    editor.className = "query_editor horizontal_split";
    document.body.appendChild(editor);

    var left_pane = document.createElement("div");
    left_pane.className = "editor_pane";
    left_pane.style.width = "30%";
    left_pane.innerHTML = "<div class='card editor'><textarea></textarea></div>"
    editor.appendChild(left_pane);

    var right_pane = document.createElement("div");
    right_pane.className = "result_pane";
    right_pane.style.left = "30%";
    right_pane.style.width = "70%";
    right_pane.innerHTML = "<div class='card'></div>"
    editor.appendChild(right_pane);

    FnordMetric.Editor.extendTextarea(left_pane.querySelector("textarea"));
  };

  renderQueryEditor();
}
