FnordMetric.views["fnordmetric.metric.list"] = function(elem, params) {

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
      },
      {
        key: "total_bytes",
        title: "Total stored bytes",
        sortable: true
      },
    ]
  });

  this.initialize = function() {
    var page = zTemplateUtil.getTemplate("fnordmetric-metric-list-tpl");
    zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(page);

    renderTable();
    table.onClick(function(r) {
      console.log(r);
    });

    loadMetricList();
  };

  this.destroy = function() {
    query_mgr.closeAll();
  };

  var loadMetricList = function() {
    var headers = {};
    zHTTP.httpGet("/api/metrics", headers, function(r) {
      if (r.status == 200) {
        var json = JSON.parse(r.response);
        renderMetricList(json);
      } else {
        renderMetricList(json);
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
      {name: "disk"},
      {name: "memcache"},
      {name: "processes"},
      {name: "kernel"}
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

};
