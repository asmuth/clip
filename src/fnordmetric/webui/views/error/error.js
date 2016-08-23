FnordMetric.views["fnordmetric.error"] = function(elem, params) {
  this.initialize = function() {
    var page = zTemplateUtil.getTemplate("fnordmetric-error-tpl");
    //zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(page);
  };

  this.destroy = function() {
  };
};
