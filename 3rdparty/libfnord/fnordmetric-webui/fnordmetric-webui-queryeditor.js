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
  var editor_resizer_tooltip;
  var current_view;
  var button_bar;

  function renderExecutionInfo(duration, tables, elem) {
    var info_field = document.createElement("div");
    info_field.className = "info_field";
    var duration = FnordMetric.util.parseMilliTS(duration);
    var rows = FnordMetric.util.humanCountRows(tables);
    info_field.innerHTML =
      "Query execution took " + duration + 
      " and returned " + rows;
    elem.appendChild(info_field);
  }

  function renderServerError(elem) {
    elem.innerHTML = "";
    var error_box = document.createElement("div");
    error_box.className = "error_box";
    error_box.innerHTML = 
      "Sorry, FnordMetric Server encountered an error. " + "<br>" + 
      "If you believe this is a bug in FnordMetric Server "+
      "please report an issue at github.com/.../issues.";
    elem.appendChild(error_box);
  }


  function runQuery(result_pane, editor_pane, view, query_str) {
    if (query_str == undefined) {
      query_str = editorViews[view].getQuery();
      var params = {
        innerView : urlName[view],
        innerViewValue : query_str};
      FnordMetric.util.setURLQueryString("query_playground", params, true, true);
    }

    if (query_str.length < 3) {
      return;
    }

    FnordMetric.util.displayLoader(result_pane);

    FnordMetric.httpPost("/query", query_str, function(r, duration) {
      if (r.status == 200 && r.statusText == "OK") {
        var res = JSON.parse(r.response);
        FnordMetric.util.queryResultView(query_str).render(
          result_pane, res, duration);
        updateLayout();
        renderExecutionInfo(duration, res.tables, result_pane);
        FnordMetric.extendCharts();
      } else {
        /* server error */
        renderServerError(result_pane);
      }
    });
  }

  function renderEditorView(view, editor_pane, result_pane, query) {
    editorViews[view].render(editor_pane, query);
    editor_pane.insertBefore(button_bar, editor_pane.firstChild);
    if (query != null && query != undefined) {
      runQuery(result_pane, editor_pane, view, query);
    }
  }


  function updateLayout(editor_width, editor_height) {
    if (direction == "horizontal") {
      var height = FnordMetric.util.getHorizontalEditorHeight(
        editor_pane.offsetHeight, result_pane.offsetHeight);

      var editor_width = (editor_width == undefined)?
        FnordMetric.util.getHorizontalEditorWidth(
          editor_pane.offsetWidth) : editor_width;

      editor_pane.style.width = editor_width + "%";
      editor_pane.style.float = "left";
      editor_pane.style.height = (height - 40) + "px";

      result_pane.style.height = height + "px";
      result_pane.style.width = (100 - editor_width) + "%";
      result_pane.style.left = editor_width + "%";
      result_pane.style.top = "";
      result_pane.style.overflowY = "auto";

      editor_resizer_tooltip.className = "editor_resizer_tooltip horizontal";
      editor_resizer_tooltip.style.left = editor_width + "%";
      editor_resizer_tooltip.style.top = editor_pane.offsetTop + "px";
      editor_resizer_tooltip.style.height = height + "px";
    } else {
      editor_pane.style.float = "";
      editor_pane.style.width = "100%";

      var editor_height = editor_height == undefined ?
        editorViews[current_view].getHeight() : editor_height;
      editor_pane.style.height = editor_height + "px";

      result_pane.style.width = "100%";
      result_pane.style.left = 0;
      result_pane.style.top = (editor_pane.offsetTop + editor_height) + "px";
      result_pane.style.height = "100%";

      editor_resizer_tooltip.className = "editor_resizer_tooltip vertical";
      editor_resizer_tooltip.style.top = (result_pane.offsetTop - 3) + "px";
      editor_resizer_tooltip.style.left = "0px";
      editor_resizer_tooltip.style.height = "5px";
    }
  }

  /* window event functions must be declared to be able to destroy them */
  function beforeUnloadEvent(e) {
      e.returnValue = 
        "You may loose your query when leaving the page.";
  }

  function dragoverEvent(e) {
    e.preventDefault();
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
    button_bar.className = "navbar";

    var editor_picker = document.createElement("div");
    editor_picker.className = "editor_type_picker";
    editor_picker.innerHTML = 
      "<i class='fa fa-database'></i> SQL Editor";
    button_bar.appendChild(editor_picker);

    var split_btn = document.createElement("i");
    split_btn.className = "fa fa-columns icobtn";
    button_bar.appendChild(split_btn);

    var refresh_btn = document.createElement("i");
    refresh_btn.className = "fa fa-refresh icobtn";
    button_bar.appendChild(refresh_btn);

    var exec_btn = document.createElement("span");
    exec_btn.className = "run_query_btn";
    exec_btn.innerHTML = "<i class='fa fa-bolt'></i> Run Query";
    button_bar.appendChild(exec_btn);


    /* init editorpane & resultpane */
    editor_pane = document.createElement("div");
    editor_pane.className = "editor_pane";
    viewport.appendChild(editor_pane);

    editor_resizer_tooltip = document.createElement("div");
    editor_resizer_tooltip.className = "editor_resizer_tooltip";
    editor_resizer_tooltip.setAttribute('draggable', 'true');
    viewport.appendChild(editor_resizer_tooltip);

    result_pane = document.createElement("div");
    result_pane.className = "result_pane";
    viewport.appendChild(result_pane);


    /* set eventListeners */
    window.addEventListener(
      'beforeunload', beforeUnloadEvent, false);

    window.addEventListener('resize', updateLayout);

    window.addEventListener('dragover', dragoverEvent, false);

    editor_pane.addEventListener('keydown', function(e) {
      if ((e.ctrlKey || e.metaKey) && e.keyCode == 13) {
        e.preventDefault();
        runQuery(
          result_pane,
          editor_pane,
          current_view);
      }
    }, false);

    editor_resizer_tooltip.addEventListener('drag', function(e) {
      editor_resizer_tooltip.style.background = "";
      if (direction == "horizontal" && e.clientX > 0) {
        var editor_width = (e.clientX / window.innerWidth) * 100;
        editor_width = Math.min(Math.max(25, editor_width), 60);
        updateLayout(editor_width, undefined);
      }

      if (direction == "vertical" && e.clientY > 0) {
        var editor_height = (e.clientY + window.pageYOffset) - editor_pane.offsetTop;
        editor_height = Math.max(100, editor_height);
        updateLayout(undefined, editor_height);
      }

    }, false);

    editor_resizer_tooltip.addEventListener('dragstart', function() {
      this.style.background = "transparent";
    }, false);


    exec_btn.onclick = function(e) {
      e.preventDefault();
      runQuery(
        result_pane,
        editor_pane,
        current_view);
    }

    split_btn.onclick = function(e) {
      e.preventDefault();
      if (direction == "horizontal") {
        direction = "vertical"
      } else {
        direction = "horizontal"
      }
      updateLayout();
    }

    refresh_btn.onclick = function() {
      var query = query_params.innerViewValue;
      query = (query != undefined)?
        encodeURIComponent(query) : "";
      FnordMetric.WebUI.singleton.openUrl(
        "query_playground?" +
        query_params.innerView + "=" +
        query);
    }

    var exec_flyout;
    exec_btn.addEventListener('mouseover', function() {
      var left = button_bar.offsetWidth;
       left = (direction == "horizontal")? 
        left - 215 : left - 370;
      var text = "Execute the query. Hint: you can alse press Ctrl/Cmd + Enter";
      exec_flyout = 
        FnordMetric.util.renderFlyout(text, viewport, left);
    }, false);

    exec_btn.addEventListener('mouseout', function() {
      FnordMetric.util.removeIfChild(
        exec_flyout, viewport);
    }, false);

    var split_flyout;
    split_btn.addEventListener('mouseover', function() {
      var text = "Change View";
      var left = editor_picker.offsetWidth - 20;
      split_flyout =
        FnordMetric.util.renderFlyout(text, viewport, left);
    }, false);

    split_btn.addEventListener('mouseout', function() {
      FnordMetric.util.removeIfChild(
        split_flyout, viewport);
    }, false);

    var refresh_flyout;
    refresh_btn.addEventListener('mouseover', function() {
      var text = "Refresh";
      var left = editor_picker.offsetWidth + split_btn.offsetWidth;
      refresh_flyout =
        FnordMetric.util.renderFlyout(text, viewport, left);
    }, false);

    refresh_btn.addEventListener('mouseout', function() {
      FnordMetric.util.removeIfChild(
        refresh_flyout, viewport);
    }, false);


    updateLayout();
    if ('innerView' in query_params) {
      current_view = urlName[query_params.innerView];
      query = (query_params.innerViewValue == "undefined") ? 
        "" : query_params.innerViewValue;
    }
    renderEditorView(current_view, editor_pane, result_pane, query);

  }

  function destroy() {
    window.removeEventListener('beforeunload', beforeUnloadEvent, false);
    window.removeEventListener('resize', updateLayout);
    window.removeEventListener('dragover', dragoverEvent, false);
  }

  return {
    "render" : render,
    "destroy" : destroy
  }
}


