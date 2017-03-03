FnordMetric.views["fnordmetric.error"] = function(elem, cfg) {
  this.initialize = function() {
    var tpl_id = cfg && cfg.type == "404" ?
        "fnordmetric-error-404-tpl" : "fnordmetric-error-tpl";
    var tpl = templateUtil.getTemplate(tpl_id);
    //DomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(tpl);
  };

  this.destroy = function() {
  };
};
