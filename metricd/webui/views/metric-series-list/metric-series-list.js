FnordMetric.views["fnordmetric.metric.series.list"] = function(elem, params) {
  'use strict';

  var api_url = "/fetch_series";
  var default_columns = [
    {
      key: "series_id",
      title: "Series ID",
      sortable: true
    },
    {
      key: "sparkline",
      title: "Value"
    },
    {
      key: "value",
      title: "",
      sortable: true
    }
  ];

  var sparkline_html = ["<z-sparkline height='40px' width='220px' ",
    "data-sparkline='{{values}}'></z-sparkline> ",
    "<span>{{latest}}</span><span>{{unit}}</span>"].join("");

  var table;
  var url_params;

  this.initialize = function() {
    var page = templateUtil.getTemplate("fnordmetric-metric-series-list-tpl");
    elem.appendChild(page);

    url_params = getParams(params.path);

    watchTimeRangePicker();
    renderHeader();
    fetchMetricSeriesList();
  };

  this.destroy = function() {

  };

/********************************* private ***********************************/

  var getParams = function(path) {
    var p = {};

    p.metric = params.route.args[1];

    var start_param = URLUtil.getParamValue(path, "start");
    if (start_param) {
      p.start = parseInt(start_param);
    }

    var end_param = URLUtil.getParamValue(path, "end");
    if (end_param) {
      p.end = parseInt(end_param);
    }

    return p;
  }

  var watchTimeRangePicker = function() {
    var picker = elem.querySelector(
        ".fnordmetric-metric-series-list f-timerange-picker");

    var timerange = {};

    var timezone = DomUtil.getCookie("timezone");
    if (timezone) {
      timerange.timezone = timezone;
    }

    if (url_params.start && url_params.end) {
      timerange.start = url_params.start;
      timerange.end = url_params.end;
    }

    picker.initialize(timerange);

    picker.addEventListener("submit", function(e) {
      updateTimezoneCookie(this.getTimezone());
      updateQueryStr(this.getTimerange());
    }, false);
  }

  var updateTimezoneCookie = function(timezone) {
    /** set the cookie expiry date for in 5 years **/
    var d = new Date();
    d.setFullYear(d.getFullYear() + 5);
    DomUtil.setCookie("timezone", timezone, d.toUTCString());
  }

  var updateQueryStr = function(query_params) {
    var url = params.path;
    for (var k in query_params) {
      url = URLUtil.addOrModifyParam(url, k, query_params[k]);
    }

    params.app.navigateTo(url);
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
    var rows = [];
    series.forEach(function(s) {
      var cells = {
        series_id: {
          value: s.series_id
        },
        sparkline: {
          value_html: sparkline_html
        }
      };

      var sparkline_values = [];
      s.values.forEach(function(v) {
        sparkline_values.push(v[1]);

        //if (!cells.max.value || cells.max.value < v[1]) {
        //  cells.max.value = v[1];
        //}

        //if (!cells.min.value || cells.min.value > v[1]) {
        //  cells.min.value = v[1];
        //}
      });

      cells.sparkline.value_html = cells.sparkline.value_html.replace(
          "{{values}}",
          sparkline_values.join(","));

      cells.sparkline.value_html = cells.sparkline.value_html.replace(
          "{{latest}}",
          s.values[s.values.length - 1][1]); //FIXME use value returned by api

      cells.sparkline.value_html = cells.sparkline.value_html.replace(
          "{{unit}}",
          s.unit || "&nbsp;MB"); //FIXME use value returned by api


      rows.push({cells: cells});
    });

    /* initialize table */
    if (!table) {
      table = new fTable({columns: default_columns});
      /* navigate to id detail page */
      table.onClick(function(r) {
        params.app.navigateTo(
            params.route.args[0] + "/" + r.cells.series_id.value);
      });

      /* sort callback */
      table.onSort(function(column, direction) {
        //FIXME handle sort
       // params.view_cfg.updateValue("order", direction);
       // params.view_cfg.updateValue("order_by", column.key);
       // updatePath();
      });
    }

    table.setRows(rows);
    table.render(elem.querySelector(
        ".fnordmetric-metric-series-list table.metric_series_list"));
   // renderPagination(series.length);
  };
};

