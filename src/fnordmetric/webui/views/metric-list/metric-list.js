FnordMetric.views["fnordmetric.metric.list"] = function(elem, params) {
  var query_mgr = EventSourceHandler();

  this.initialize = function(params) {
    var page = zTemplateUtil.getTemplate("fnordmetric.metric.list.tpl");
    elem.appendChild(page);

    fetchMetricList();
  };

  this.destroy = function() {
    query_mgr.closeAll();
  };

  var fetchMetricList = function() {

  };

};
