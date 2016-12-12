FnordMetric.views["fnordmetric.metric.series.list"] = function(elem, params) {
  'use strict';

  var api_url = "/fetch_series";
  var default_columns = [
    {
      key: "min",
      title: "Min"
    },
    {
      key: "max",
      title: "Max"
    },
    {
      key: "sparkline",
      title: ""
    }
  ];



  var table;
  var url_params;

  this.initialize = function() {
    var page = templateUtil.getTemplate("fnordmetric-metric-series-list-tpl");
    elem.appendChild(page);

    url_params = getParams(params.path);

    renderHeader();
    fetchMetricSeriesList();
  };

  this.destroy = function() {

  };

/********************************* private ***********************************/

  var getParams = function(path) {
    var p = {};

    p.metric = params.route.args[1];
    var cfg_param = URLUtil.getParamValue(path, "cfg");
    if (cfg_param) {
      p.config = cfg_param;
    }

    var offset_param = URLUtil.getParamValue(path, "offset");
    if (offset_param) {
      p.offset = offset_param;
    }

    return p;
  }

  var renderHeader = function() {
    var header = elem.querySelector(
        ".fnordmetric-metric-series-list .page_header .metric_name");
    header.innerHTML = url_params.metric;
  }

  var fetchMetricSeriesList = function() {
    var query_str = [
      "metric_id=",
      url_params.metric
    ].join("");

    var url = params.app.api_base_path + api_url + "?" + query_str;
    HTTPUtil.httpGet(url, {}, function(r) {
      if (r.status != 200) {
        params.app.renderError(
            "an error occured while loading the metric series list:",
            r.response);
        return;
      }

      var series = JSON.parse(r.response);
      renderTable(series.series);
    });
  };

  var renderTable = function(series) {
    /* build columns and rows */
    var columns = [];
    var column_keys = {};
    var rows = [];
    series.forEach(function(s) {
      var cells = {
        sparkline: {
          value_html: "<z-sparkline height='20px' width='100px' data-sparkline='{{values}}'></sparkline>"
        },
        max: {
          value: null
        },
        min: {
          value: null
        }
      }

      var sparkline_values = [];
      s.values.forEach(function(v) {
        sparkline_values.push(v[1]);

        if (!cells.max.value || cells.max.value < v[1]) {
          cells.max.value = v[1];
        }

        if (!cells.min.value || cells.min.value > v[1]) {
          cells.min.value = v[1];
        }
      });

      cells.sparkline.value_html = cells.sparkline.value_html.replace(
          "{{values}}",
          sparkline_values.join(","));

      for (var key in s.labels) {
        cells[key] = {value : s.labels[key]};

        if (!column_keys.hasOwnProperty(key)) {
          column_keys[key] = true;
          columns.push({
            key: key,
            title: key
          });
        }
      }

      rows.push({cells: cells});
    });

    /* initialize table */
    if (!table) {
      table = new fTable({columns: columns.concat(default_columns)});
      /* navigate to id detail page */
      table.onClick(function(r) {
        params.app.navigateTo(params.route.args[0] + "/" + r.cells.sensor.value);
      });

      /* sort callback */
      table.onSort(function(column, direction) {
        params.view_cfg.updateValue("order", direction);
        params.view_cfg.updateValue("order_by", column.key);
        updatePath();
      });
    }

    table.setRows(rows);
    table.render(elem.querySelector(
        ".fnordmetric-metric-series-list table.metric_series_list"));
   // renderPagination(series.length);
  };
};

