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

FnordMetric.views.QueryPlayground = function() {
  var editorViews = {
    "sql" : FnordMetric.util.SQLEditorView,
    "visual" : FnordMetric.util.VisualEditorView
  }

  function renderEditorView(view, editor_pane) {
    editorViews[view].render(editor_pane);
  }

  function renderResult(result_pane) {

  }

  function updateLayout(editor_pane, result_pane, direction) {

  }

  function render(viewport, url) {
    var direction = "horizontal";
    //init viewport
    viewport.innerHTML = "";

    //render buttons
    var button_bar = document.createElement("div");
    button_bar.className = "navbar";
    var split_btn = FnordMetric.createButton(
      "#", "fancy_button", "Change View");
    var query_btn = FnordMetric.createButton(
      "#", "fancy_button", "Run Query");
    var embed_btn = FnordMetric.createButton(
      "#", "fancy_button", "Embed Query");

    button_bar.appendChild(split_btn);
    button_bar.appendChild(query_btn);
    button_bar.appendChild(embed_btn);
    viewport.appendChild(button_bar);

    //init editorpane & resultpane
    var editor_pane = document.createElement("div");
    editor_pane.className = "editor_pane";
    viewport.appendChild(editor_pane);

    var result_pane = document.createElement("div");
    result_pane.className = "result_pane";
    viewport.appendChild(result_pane);

    updateLayout(editor_pane, result_pane, direction);

    //first Version --> later the editor may be defined in the url
    renderEditorView("sql", editor_pane);

  }


  function destroy() {

  }

  return {
    "render" : render,
    "destroy" : destroy
  }
}


