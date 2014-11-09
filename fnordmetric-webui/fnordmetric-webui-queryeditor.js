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
    "sql" : FnordMetric.util.SQLEditorView(),
    "visual" : FnordMetric.util.VisualEditorView()
  }

  function renderEditorView(view, editor_pane) {
    editorViews[view].render(editor_pane);
  }

  function renderResult(result_pane, editor_pane, direction, view) {
    var query_str = editorViews[view].getQuery();
    //render Loading
    FnordMetric.httpPost("/query", query_str, function(r, duration) {
      if (r.status == 200 && r.statusText == "Ok") {
        var res = JSON.parse(r.response);
        FnordMetric.util.queryResultView().render(
          result_pane, res, duration);
        updateLayout(editor_pane, result_pane, direction);
      } else {
        //renderError(r);
      }
    });
  }

  function updateLayout(editor_pane, result_pane, direction) {
    if (direction == "horizontal") {
      var height = FnordMetric.util.getHorizontalEditorHeight(
        editor_pane.offsetHeight, result_pane.offsetHeight);
      var editor_width =
        FnordMetric.util.getHorizontalEditorWidth(
          editor_pane.offsetWidth);
      editor_pane.style.width = editor_width + "%";
      editor_pane.style.float = "left";
      editor_pane.style.height = height + "px";
      result_pane.style.height = height + "px";
      result_pane.style.width = (99 - editor_width) + "%";
      result_pane.style.left = editor_width + "%";
      result_pane.style.top = "";
      result_pane.style.overflowY = "auto";
    } else {
      editor_pane.style.float = "";
      editor_pane.style.width = "100%";
      /*editor_pane.style.height = editor_height + "px";
      query_editor.style.height = editor_height + "px";*/
      result_pane.style.width = (window.innerWidth - 55) + "px";
      result_pane.style.left = "20px";
      //result_pane.style.top = (editor_pane.offsetTop + editor_height) + "px";
      result_pane.style.height = "auto";
    }
  }

  function render(viewport, url) {
    var direction = "horizontal";
    var current_view = "sql";
    /* init viewport */
    viewport.innerHTML = "";

    /* render buttons */
    var button_bar = document.createElement("div");
    button_bar.className = "navbar";
    var split_btn = FnordMetric.createButton(
      "#", "fancy_button", "Change View");

    var query_btn = FnordMetric.createButton(
      "#", "fancy_button", "Run Query");
    query_btn.onclick = function(e) {
      e.preventDefault();
      renderResult(result_pane, editor_pane, direction, current_view);
    }

    var embed_btn = FnordMetric.createButton(
      "#", "fancy_button", "Embed Query");

    button_bar.appendChild(split_btn);
    button_bar.appendChild(query_btn);
    button_bar.appendChild(embed_btn);
    viewport.appendChild(button_bar);

    /* init editorpane & resultpane */
    editor_pane = document.createElement("div");
    editor_pane.className = "editor_pane";
    viewport.appendChild(editor_pane);

    result_pane = document.createElement("div");
    result_pane.className = "result_pane";
    viewport.appendChild(result_pane);

    updateLayout(editor_pane, result_pane, direction);

    /* first Version --> later the editor may be defined in the url */
    renderEditorView(current_view, editor_pane);

  }


  function destroy() {

  }

  return {
    "render" : render,
    "destroy" : destroy
  }
}


