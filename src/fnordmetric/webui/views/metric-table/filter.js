FnordMetric.MetricTableFilter = function(elem) {
  var submit_callbacks = [];

  this.onSubmit = function(callback) {
    submit_callbacks.push(callback);
  }

  this.render = function(filter_str) {
    var inner = elem.querySelector(".inner");
    zDomUtil.clearChildren(inner);

    var tpl = zTemplateUtil.getTemplate(
        "fnordmetric-metric-table-filter-modal-tpl");

    if (filter_str) {
      tpl.querySelector(".input_container input.filter").value = filter_str;
    }

    tpl.querySelector("button.close").addEventListener("click", function() {
      elem.close();
    }, false);

    tpl.querySelector("button.submit").addEventListener("click", function() {
      var filter = elem.querySelector(".input_container input.filter").value;
      if (filter.length == 0) {
        alert("error: please provide a filter string");
        //FIXME render error
        return;
      }

      submit_callbacks.forEach(function(callback) {
        console.log("ddd");
        callback(filter)
      });

      elem.close();
    }, false);

    inner.appendChild(tpl);

    elem.show();
  }

}
