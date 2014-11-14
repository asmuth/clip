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

  var urlName = {
    "sql" : "sql_query",
    "sql_query" : "sql",
    "visual" : "visual_query",
    "visual_query" : "visual"
  }

  var viewport;
  var direction;
  var editor_pane;
  var result_pane;
  var current_view;
  var button_bar;

  function renderExecutionInfo(duration, tables, elem) {
    if (tables == undefined) {return;}
    if (elem.lastChild.className == "info_field") {
      elem.removeChild(elem.lastChild);
    }
    var info_field = document.createElement("div");
    info_field.className = "info_field";
    var duration = FnordMetric.util.parseMilliTS(duration);
    var rows = FnordMetric.util.humanCountRows(tables);
    info_field.innerHTML =
      "The execution took " + duration + 
      " and returned " + rows;
    info_field.style.top = elem.offsetHeight + "px";
    elem.appendChild(info_field);
  }

  function renderServerError(elem) {
    elem.innerHTML = "";
    var error_box = document.createElement("div");
    error_box.className = "error_box";
    error_box.innerHTML = 
      "Oopps. FnordMetric Server encountered an error. " + "<br>" + 
      "If you believe this is a bug in FnordMetric Server "+
      "please report an issue at github.com/.../issues.";
    elem.appendChild(error_box);
  }


  function runQuery(result_pane, editor_pane, view, query_str) {
    if (query_str == undefined) {
      query_str = editorViews[view].getQuery();
      FnordMetric.util.setURLQueryString(urlName[view], query_str, true);
    }

    FnordMetric.util.displayLoader(result_pane);

    FnordMetric.httpPost("/query", query_str, function(r, duration) {
      if (r.status == 200 && r.statusText == "Ok") {
        var res = JSON.parse(r.response);
        FnordMetric.util.queryResultView().render(
          result_pane, res, duration);
        updateLayout(editor_pane, result_pane, direction);
        renderExecutionInfo(duration, res.tables, result_pane);
        FnordMetric.extendCharts();
      } else {
        /* server error */
        renderServerError(result_pane);
      }
    });
  }

  function renderEditorView(view, editor_pane, result_pane, query) {
    console.log(view);
    editorViews[view].render(editor_pane, query);
    editor_pane.insertBefore(button_bar, editor_pane.firstChild);
    if (query != undefined) {
      runQuery(result_pane, editor_pane, view, query);
    }
  }


  function updateLayout() {
    if (direction == "horizontal") {
      var height = FnordMetric.util.getHorizontalEditorHeight(
        editor_pane.offsetHeight, result_pane.offsetHeight);
      var editor_width =
        FnordMetric.util.getHorizontalEditorWidth(
          editor_pane.offsetWidth);
      editor_pane.style.width = editor_width + "%";
      editor_pane.style.float = "left";
      editor_pane.style.height = (height - 40) + "px";
      result_pane.style.height = height + "px";
      result_pane.style.width = (100 - editor_width) + "%";
      result_pane.style.left = editor_width + "%";
      result_pane.style.top = "";
      result_pane.style.overflowY = "auto";
    } else {
      editor_pane.style.float = "";
      editor_pane.style.width = "100%";
      var editor_height = editorViews[current_view].getHeight();
      editor_pane.style.height = editor_height + "px";
      result_pane.style.width = "100%";
      result_pane.style.left = 0;
      result_pane.style.top = (editor_pane.offsetTop + editor_height) + "px";
      result_pane.style.height = "100%";
    }
  }

  function render(viewport, url, query_params) {
    direction = "horizontal";
    viewport = viewport;
    current_view = "sql";
    var query = null;
    /* init viewport */
    viewport.innerHTML = "";

    /* render buttons */
    button_bar = document.createElement("div");
    //button_bar.innerHTML = "<div class='editor_type_picker'><i class='fa fa-database'></i> SQL Editor <i class='fa fa-chevron-down'></i></div>";
    button_bar.innerHTML = "<div class='editor_type_picker'><i class='fa fa-database'></i> SQL Editor</div>";
    button_bar.className = "navbar";

    var split_btn = document.createElement("i");
    split_btn.className = "fa fa-columns icobtn";
    button_bar.appendChild(split_btn);

    split_btn.onclick = function(e) {
      e.preventDefault();
      if (direction == "horizontal") {
        direction = "vertical"
      } else {
        direction = "horizontal"
      }
      updateLayout();
      console.log("change view");
    }

    var refresh_btn = document.createElement("i");
    refresh_btn.className = "fa fa-refresh icobtn";
    button_bar.appendChild(refresh_btn);

    var exec_btn = document.createElement("span");
    exec_btn.className = "run_query_btn";
    exec_btn.innerHTML = "<i class='fa fa-bolt'></i> Run Query";
    button_bar.appendChild(exec_btn);

    exec_btn.onclick = function(e) {
      e.preventDefault();
      runQuery(
        result_pane,
        editor_pane,
        current_view);
    }


/*
    var embed_btn = FnordMetric.createButton(
      "#", "fancy_button", "Embed Query");

    embed_btn.onclick = function(e) {
      e.preventDefault();
      FnordMetric.util.openPopup(
        viewport, "Todo: Ruby/JS/html snippet");
    }
*/
    //button_bar.appendChild(query_btn);
    //button_bar.appendChild(embed_btn);

    /* init editorpane & resultpane */
    editor_pane = document.createElement("div");
    editor_pane.className = "editor_pane";
    viewport.appendChild(editor_pane);

    /* set eventListeners */
    editor_pane.addEventListener('keydown', function(e) {
      if ((e.ctrlKey || e.metaKey) && e.keyCode == 13) {
        e.preventDefault();
        runQuery(
          result_pane,
          editor_pane,
          current_view);
      }
    }, false);

    /* in dev mode disabled */
    /*window.onbeforeunload = function(e) {
      return "You may loose your query when leaving the page.";
    }*/


    result_pane = document.createElement("div");
    result_pane.className = "result_pane";
    viewport.appendChild(result_pane);

    updateLayout(editor_pane, result_pane, direction);

    if (query_params != undefined) {
      current_view = urlName[query_params.innerView];
      query = query_params.innerviewValue;
    }
    renderEditorView(current_view, editor_pane, result_pane, query);
  }


  function destroy() {

  }

  return {
    "render" : render,
    "destroy" : destroy
  }
}


