/*
  This file is part of the "FnordMetric" project
    Copyright (c) 2014 Laura Schlimmer
    Copyright (c) 2014 Paul Asmuth, Google Inc.

  FnordMetric is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License v3.0. You should have received a
  copy of the GNU General Public License along with this program. If not, see
  <http://www.gnu.org/licenses/>.
*/

/*
  handles metric controls in metric-explorer-preview
*/
if (typeof MetricHandler == "undefined") {
  MetricHandler = {};
}

MetricHandler.setColumns = function(metric_control) {
  var url = "/metrics/list?filter=" + encodeURIComponent(metric_control.params.name);
  metric_control.params.group_by = this.getMetricUrlParamOrDefault(
    "group_by", metric_control.params.index);

  Fnord.httpGet(url, function(r) {
    if (r.status == 200) {
      var json = JSON.parse(r.response);
      json.metrics.map(function(m) {
        metric_control.params.columns = m.labels.join(",");
      });
      metric_control.columnsReloaded();
    }
  });
};


MetricHandler.initControl = function(metric, name, index) {
  var base = this;

  if (!index) {
    index = metric.getAttribute('data-index');
  }

  var aggr_fn = this.getMetricUrlParamOrDefault("aggr_fn", index);

  var metric_params = {
    "name": name,
    "aggr_fn": aggr_fn,
    "scale": this.getMetricUrlParamOrDefault("scale", index),
    "index" : index
  }

  metric_params.aggr_window =
    this.getMetricUrlParamOrDefault("aggr_window", index);
  metric_params.aggr_step =
    this.getMetricUrlParamOrDefault("aggr_step", index);


  if (this.params["metric" + index] == undefined) {
    this.params["metric" + index] = metric_params;
    this.params.metrics += 1;
    this.update();
  }

  metric.params = metric_params;
  metric.setAttribute('data-index', index);
  metric.setAttribute('data-state', 'initialised');

  if (metric.params.name) {
    //reload metric columns
    metric_params.columns = MetricHandler.setColumns.apply(this, [metric]);
  }

  // handle events
  metric.addEventListener(
    "fn-metric-control-attr-changed", function(e) {
      base.metricParamChanged(this.params, e.detail.key);
    },
  false);

  if (index > 0) {
    metric.addEventListener('fn-metric-control-remove-metric', function() {
      MetricHandler.removeControl.apply(base, [this.getAttribute('data-index')]);
    }, false);
  } else {
    metric.addEventListener('fn-metric-control-add-metric', function() {
      MetricHandler.renderSelectModal.apply(base, []);
    }, false);
  }
};

MetricHandler.addControl = function(index) {
  var base = this;
  var controls = this.querySelectorAll("fn-metric-control");
  var new_control = document.createElement("fn-metric-control");
  var index = index;

  if (!index && controls) {
    index = parseInt(
      controls[controls.length - 1].getAttribute('data-index'), 10) + 1;
  }

  new_control.setAttribute('data-state', 'select-metric');
  new_control.setAttribute('data-index', index);

  this.querySelector(".metric-controls").appendChild(new_control);

  //only call onResize if DOMContent has already been loaded
  if (this.DOMLoaded) {
    this.onResize();
  }

  return new_control;
};

MetricHandler.removeControl = function(index) {
  var metric_control =
    this.querySelector("fn-metric-control[data-index='" + index + "']");

  metric_control.parentNode.removeChild(metric_control);
  this.onResize();

  if (this.params["metric" + index]) {
    //metric has been initialised
    delete this.params["metric" + index];
    this.params.metrics = this.params.metrics - 1;
  }

  this.update();
};

MetricHandler.renderSelectModal = function() {
  var base = this;
  var modal = this.querySelector("fn-modal[name='select_metric']");
  var table = modal.querySelector("fn-table");
  table.querySelector("table").innerHTML = "";

  Fnord.httpGet('/metrics/list', function(r) {
    if (r.status == 200) {
      var metrics = JSON.parse(r.response).metrics;
      metrics.forEach(function(metric) {
        var tr_elem = document.createElement("tr");
        tr_elem.setAttribute('metric-key', metric.key);
        tr_elem.innerHTML = "<td>" + metric.key + "</td>";
        table.appendRow(tr_elem);
      });
    }
    modal.show();
  });

  if (!table.rowClickListener) {
    table.rowClickListener = true;

    table.addEventListener('fn-table-row-click', function(e) {
      var key = e.srcElement.getAttribute('metric-key');
      var control = MetricHandler.addControl.apply(base, [null, true]);
      MetricHandler.initControl.apply(base, [control, key]);
      modal.close();
    }, false);
  }
};
