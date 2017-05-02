FnordMetric.views["fnordmetric.metric.serie"] = function(elem, params) {
  'use strict';

  var api_path = "/fetch_series";
  var path = params.path;
  var url_params;
  var viewport_elem;
  var view_cfg;

  this.initialize = function() {
    url_params = getParams(params.path);
    var page = templateUtil.getTemplate("fnordmetric-metric-serie-tpl");

    DomUtil.handleLinks(page, params.app.navigateTo);
    DomUtil.replaceContent(elem, page);
    viewport_elem = elem.querySelector(".view_content");

    render();
    fetchData();
  }

  var updatePath = function(query_params) {
    var url = params.path;
    for (var k in query_params) {
      url = URLUtil.addOrModifyParam(url, k, query_params[k]);
    }

    params.app.navigateTo(url);
  }

  var getParams = function(path) {
    var p = {};

    p.metric = params.route.args[1];
    p.series_id = params.route.args[2];

    /** param view **/
    var view_param = URLUtil.getParamValue(path, "view");
    if (view_param) {
      p.view = view_param;
    } else {
      p.view = "table";
    }

    /** param compare_to **/
    var compare_to_param = URLUtil.getParamValue(path, "compare_to");
    if (compare_to_param) {
      p.compare_to = compare_to_param;
    }

    /** param offset **/
    var offset_param = URLUtil.getParamValue(path, "offset");
    if (offset_param) {
      p.offset = offset_param;
    }

    return p;
  }

  var fetchData = function() {
    var url = params.app.api_base_path + api_path + "?metric_id=" + url_params.metric; //FIXME 
    HTTPUtil.httpGet(url, {}, function(r) {
      if (r.status != 200) {
        params.app.renderError(
            "an error occured while loading the metric list:",
            r.response);
        return;
      }

      var serie;
      var series = JSON.parse(r.response).series;
      for (var i = 0; i < series.length; i++) {
        if (series[i].series_id == url_params.series_id) {
          serie = series[i];
          break;
        }
      }

      if (!serie) {
        //TODO handle empty result list
      } else {
        renderSerieLabels(serie.labels);
        renderView(serie.values);
      }
    });
  }

  var render = function() {
    renderHeader();
    // renderView(); --> initView
    renderTimerangeControl();
    watchTimeRangePicker();
    renderEmbedControl();
  };

  var renderHeader = function(metric) {
    var metric_title = elem.querySelector(
        ".fnordmetric-metric-serie .page_header .metric_name");
    metric_title.innerHTML = url_params.metric;
    metric_title.setAttribute(
        "href",
        "/metrics/" + encodeURIComponent(url_params.metric));

    /* handle view controls */
    elem.querySelector(".fnordmetric-metric-serie .view_control").setAttribute(
        "data-view", url_params.view);

    /* switch to table view */
    elem.querySelector(
        ".fnordmetric-metric-serie .view_control .table_view").
        addEventListener("click", function(e) {
          updatePath({view: "table"});
        }, false);

    /* switch to timeseries view */
    elem.querySelector(
        ".fnordmetric-metric-serie .view_control .timeseries_view").
        addEventListener("click", function(e) {
          updatePath({view: "timeseries"});
        }, false);
  };

  var renderSerieLabels = function(labels) {
    var title = [];

    for (var label in labels) {
      title.push(label + ": " + labels[label]);
    }

    elem.querySelector(".fnordmetric-metric-serie .page_header .serie_name").
        innerHTML = DomUtil.escapeHTML(title.join(", "));
  }

  var watchTimeRangePicker = function() {
    var picker = elem.querySelector(
        ".fnordmetric-metric-serie f-timerange-picker");

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
      updatePath(this.getTimerange());
    }, false);
  }

  var renderTimerangeControl = function() {
    var dropdown = elem.querySelector(
        ".fnordmetric-metric-serie .control f-dropdown.timerange");

    var compare_to_value = url_params.compare_to;
    if (compare_to_value != null) {
      dropdown.setValue(compare_to_value);
    }

    dropdown.addEventListener("select", function(e) {
      updatePath({compare_to: e.detail.value});
    }, false);
  }

  var renderView = function(results) {
    //TODO enable / disable current view icon in header
    var view;
    var view_opts = {
      data: results
    };

    switch (url_params.view){
      case "table":
        view = FnordMetric.views["fnordmetric.metric.serie.table"];
        break;

      case "timeseries":
        view = FnordMetric.views["fnordmetric.metric.serie.timeseries"];
        break;

      default:
        view = FnordMetric.views["fnordmetric.404"];
        break;
    }

    //render view
    viewport_elem.innerHTML = "";

    var current_view = {};
    view.call(current_view, viewport_elem, view_opts);
    if (current_view.initialize) {
      current_view.initialize.call(current_view);
    }
  }

  var renderEmbedControl = function() {
    elem.querySelector(".fnordmetric-metric-serie .control.embed")
        .addEventListener("click", function() {
          fEmbedPopup(elem, "").render();
        }, false);
  }
}

