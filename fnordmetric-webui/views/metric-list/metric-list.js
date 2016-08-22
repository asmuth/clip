FnordMetric.views["fnordmetric.metric.list"] = function(elem, params) {
  var query_mgr = EventSourceHandler();

  this.initialize = function(params) {
    var page = zTemplateUtil.getTemplate("");

  };

  this.destroy = function() {
    query_mgr.closeAll();
  };


};
