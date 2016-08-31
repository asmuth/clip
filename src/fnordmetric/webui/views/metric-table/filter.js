FnordMetric.MetricTableFilter = function(elem) {
  var submit_callbacks = [];
  var change_callbacks = [];
  var delete_callbacks = [];

  /** add a new filter */
  this.onSubmit = function(callback) {
    submit_callbacks.push(callback);
  }

  /** change an exisiting filter */
  this.onChange = function(callback) {
    change_callbacks.push(callback);
  }

  /** delete an existing filter */
  this.onDelete = function(callback) {
    delete_callbacks.push(callback);
  }

  this.render = function(filter_str) {
    var inner = elem.querySelector(".inner");
    zDomUtil.clearChildren(inner);

    var tpl = zTemplateUtil.getTemplate(
        "fnordmetric-metric-table-filter-modal-tpl");

    tpl.querySelector("button.close").addEventListener("click", function() {
      elem.close();
    }, false);

    tpl.querySelector("button.submit").addEventListener("click", function() {
      var new_filter_str = elem.querySelector(".input_container input.filter").value;
      if (new_filter_str.length == 0) {
        alert("error: please provide a filter string");
        //FIXME render error
        return;
      }

      if (filter_str) {
        //existing filter has been changed
        if (new_filter_str != filter_str) {
          change_callbacks.forEach(function(cb) {
            cb(new_filter_str, filter_str);
          });
        }

      } else {
        //new filter has been created
        submit_callbacks.forEach(function(cb) {
          cb(filter)
        });
      }

      elem.close();
    }, false);


    /* eixsiting filter */
    if (filter_str) {
      tpl.querySelector(".input_container input.filter").value = filter_str;
      tpl.querySelector("button.delete").addEventListener("click", function(e) {
        delete_callbacks.forEach(function(cb) {
          cb(filter_str);
        });
      }, false);
    }


    inner.appendChild(tpl);

    elem.show();
  }

}
