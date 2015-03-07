/*
  This file is part of the "FnordMetric" project
    Copyright (c) 2014 Laura Schlimmer
    Copyright (c) 2014 Paul Asmuth, Google Inc.

  FnordMetric is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License v3.0. You should have received a
  copy of the GNU General Public License along with this program. If not, see
  <http://www.gnu.org/licenses/>.
*/

if (typeof PreviewRenderer == "undefined") {
  PreviewRenderer = {};
}

PreviewRenderer.hideActiveResult = function() {
  var active_elem = this.querySelector("fn-loader[name='result_pane'] .active")
  if (active_elem) {
    active_elem.classList.remove("active");
  }
};

PreviewRenderer.renderChart = function(resp) {
  var pane = this.querySelector(".result_pane[data-format='svg']");
  PreviewRenderer.hideActiveResult.apply(this, []);
  pane.classList.add("active");
  pane.innerHTML = "";
  pane.innerHTML = resp;
  this.querySelector("fn-loader[name='result_pane']").removeAttribute("data-loading");
};

PreviewRenderer.renderCsvAsTable = function(csv) {
  var base = this;
  var pane = this.querySelector(".result_pane[data-format='csv']");
  var fn_table = pane.querySelector("fn-table");
  var table = fn_table.querySelector("table");
  var pager = pane.querySelector("fn-pager");
  var data_points = csv.split(/\n/);
  var row_count = 0;

  PreviewRenderer.hideActiveResult.apply(this, []);
  pane.classList.add("active");

  table.innerHTML = "";

  data_points.forEach(function(point) {
    if (point.length > 0) {
      var entries = point.split(";");
      var tr_elem = document.createElement("tr");
      entries.forEach(function(entry) {
        var td_elem = document.createElement("td");
        td_elem.innerHTML = entry;
        tr_elem.appendChild(td_elem);
      });

      table.appendChild(tr_elem);
      row_count++;
    }
  });


  pager.setAttribute('data-last-item', row_count);
  pager.forElement(fn_table);
  fn_table.renderTable();

  this.querySelector("fn-loader[name='result_pane']").removeAttribute(
    "data-loading");

  PreviewRenderer.setTableHeight.apply(this, []);
};

PreviewRenderer.setTableHeight = function() {
  var fn_table = this.querySelector(".result_pane fn-table");
  var pager = this.querySelector(".result_pane fn-pager");
  var data_per_page;

  var height =
    parseInt(this.getGenericParamOrDefault("height"), 10) -
    pager.offsetHeight;


  data_per_page = fn_table.setDataPerPageForHeight(height);
  pager.setAttribute('data-per-page', data_per_page);
};

PreviewRenderer.renderError = function(response) {
  var loader = this.querySelector("fn-loader[name='result_pane']");
  var msg_elem = loader.querySelector("fn-message");
  var header_elem = document.createElement("fn-message-header");
  var text_elem = document.createElement("fn-message-text");

  header_elem.innerHTML = response.statusText;
  text_elem.innerHTML =
    "statusCode: " + response.status + " " + response.response;
  msg_elem.innerHTML = "";
  msg_elem.appendChild(header_elem);
  msg_elem.appendChild(text_elem);

  PreviewRenderer.hideActiveResult.apply(this, []);
  msg_elem.classList.add("active");
  loader.removeAttribute("data-loading");
};
