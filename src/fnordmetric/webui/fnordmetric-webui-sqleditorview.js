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

if (FnordMetric.util === undefined) {
  FnordMetric.util = {};
}

FnordMetric.util.SQLEditorView = function() {
  var cm;
  function getHeight() {
    var line_num = cm.lineCount();
    return (line_num * 18 + 200);
  }

  function getQuery() {
    var query = cm.getValue();
    return query;
  }


  function render(elem, query) {
    //elem.style.border = "1px solid #ddd";
    //init editor_pane
    elem.innerHTML = "";

    cm = CodeMirror(elem, {
      lineNumbers: true,
      lineWrapping: true,
      autofocus: true,
      mode: "text/x-chartsql"
    });

    if (query != undefined) {
      cm.setValue(query);
    }
  }
  return {
    "render" : render,
    "getQuery" : getQuery,
    "getHeight" : getHeight
  }
}
