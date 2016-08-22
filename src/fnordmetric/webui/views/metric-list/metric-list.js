FnordMetric.views["fnordmetric.metric.list"] = function(elem, params) {

  this.initialize = function(params) {
    var page = zTemplateUtil.getTemplate("fnordmetric-metric-list-tpl");
    elem.appendChild(page);

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
        FnordMetric.renderError("An error occured while loading the metric list");
      }
    });
  };

};
