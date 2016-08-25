FnordMetric.views["fnordmetric.list"] = function(elem, params) {
  var url_params = {};
  var table = new zTable({
    columns: [
      {
        key: "metric_name",
        title: "Metric",
        sortable: true
      },
      {
        key: "labels",
        title: "Labels",
        sortable: true
      },
      {
        key: "last_insert",
        title: "Last Insert",
        sortable: true
      }
    ]
  });

  this.initialize = function() {
    url_params = getUrlParams(params.path);

    var page = zTemplateUtil.getTemplate("fnordmetric-metric-list-tpl");
    renderHeader(page);

    zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(page);


    renderTable();
    table.onClick(function(r) {
      params.app.navigateTo("/metrics/" + r.metric_id);
    });

    loadMetricList();
  };

  this.destroy = function() {
    //query_mgr.closeAll();
  };

  var getUrlParams = function(path) {
    var p = {};

    var query_param = zURLUtil.getParamValue(path, "q");
    if (query_param) {
      p.query = query_param;
    }

    return p;
  };

  var renderHeader = function(page) {
    var header = page.querySelector(".page_header .metric_selection");
    if (url_params.hasOwnProperty("query")) {
      header.innerHTML = "Search for &quot;" + url_params.query + "&quot";
    } else {
      header.innerHTML = "All Metrics"
    }
  };

  var loadMetricList = function() {
    var headers = {};
    //REMOVE ME 
    renderMetricList();
    return;
    //REMOVE ME END
    zHTTP.httpGet("/api/metrics", headers, function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        if (json.length == 0) {
          renderEmptyList();
        } else {
          renderMetricList(json);
        }

      } else {
        FnordMetric.renderError("An error occured while loading the metric list");
      }
    });
  };

  var renderTable = function() {
    table.render(elem.querySelector("table.metric_list"));
  };

  var renderMetricList = function(metric_list) {
    var tbody = elem.querySelector("table.metric_list tbody");
    zDomUtil.clearChildren(tbody);

    //REMOVEME metric list stub
    metric_list = [
      {name: "unix-stats"},
      {name: "memcache"},
    ];
    //REMOVEME END

    metric_list.forEach(function(metric) {
      table.addRow({
        metric_id: metric.name,
        cells: {
          metric_name: {
            value: metric.name
          }
        }
      });
    });

    renderTable();
  }

  var renderEmptyList = function() {
    var class_name = url_params.hasOwnProperty("query") ?
        "empty_search" : "empty_result";

    elem.querySelector(".fnordmetric-metric-list .error." + class_name)
        .classList.remove("hidden");
    elem.querySelector(".fnordmetric-metric-list table.metric_list")
        .classList.add("hidden");
  }

};
