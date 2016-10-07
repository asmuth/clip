FnordMetric.views["fnordmetric.error"] = function(elem, cfg) {
  this.initialize = function() {
    var tpl_id = cfg && cfg.type == "404" ?
        "fnordmetric-error-404-tpl" : "fnordmetric-error-tpl";
    var tpl = zTemplateUtil.getTemplate(tpl_id);
    //zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(tpl);
  };

  this.destroy = function() {
  };
};
