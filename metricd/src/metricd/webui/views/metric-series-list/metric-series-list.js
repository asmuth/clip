FnordMetric.views["fnordmetric.metric.series.list"] = function(elem, params) {
  'use strict';

  var api_url = "/fetch";

  var url_params;

  this.initialize = function() {
    var page = templateUtil.getTemplate("fnordmetric-metric-series-list-tpl");
    elem.appendChild(page);

    url_params = getParams(params.path);

    watchTimeRangePicker();
    watchFilter();
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

    var filter_param = URLUtil.getParamValue(path, "filter");
    if (filter_param) {
      p.filter = filter_param;
    }

    return p;
  }

  var updateQueryStr = function(query_params) {
    var url = params.path;
    for (var k in query_params) {
      url = URLUtil.addOrModifyParam(url, k, query_params[k]);
    }

    params.app.navigateTo(url);
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

  var watchFilter = function() {
    var filter_input = elem.querySelector(".search input");
    if (url_params.filter) {
      filter_input.value = DomUtil.escapeHTML(url_params.filter);
    }

    DomUtil.onEnter(filter_input, function(e) {
      updateQueryStr({filter: filter_input.value});
    }, false);
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
      new FnordMetric.SeriesTable(
          elem.querySelector("table.metric_series_list"),
          series.series);
      renderChart(series.series[0]);
    });
  };

  var renderChart = function(series) {
    //FIXME make another request as soon as API is ready
    new FnordMetric.SeriesChart(elem.querySelector(".chart_pane"), {
      time: series.time,
      summary: {
        series_id: series.series_id
      },
      series: [
        {
          series_id: series.series_id,
          values: series.values,
          summaries: series.summaries,
          time: series.time,
          //REMOVEME
          title: "Current Value",
          unit: "MB/s"
          //REMOVEME END
        },
          //REMOVEME
        {
          series_id: "test",
          title: "Compare To: Yesterday",
          values: series.values.map(function(v) { return v - 1000 } ),
          time: series.time,
          unit: "MB/s"
        }
          //REMOVEME END
      ]
    });
    
  }

};

