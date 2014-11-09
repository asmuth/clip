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
  function initCodeMirror(cm) {
    /* set CodeMirror Configs here */
  }

  function getQuery() {
    var query = cm.getValue();
    return query;
  }


  function render(elem) {
    //elem.style.border = "1px solid #ddd";
    //init editor_pane
    elem.innerHTML = "";

    cm = CodeMirror(elem, {
      lineNumbers: true,
      lineWrapping: true,
      autofocus: true
    });

    initCodeMirror(cm);

    
  }
  return {
    "render" : render,
    "getQuery" : getQuery
  }
}
