FnordMetric.MetricTableViewConfig = function(uri_params) {
  var default_limit = 30;
  var default_offset = 0;
  var default_order = "desc";
  var default_view = "table";

  var cfg = {};

  this.get = function() {
    return cfg;
  }

  this.getValue = function(key) {
    if (cfg.hasOwnProperty(key)) {
      return cfg[key];
    } else {
      return null;
    }
  }

  this.updateValue = function(key, value) {
    cfg[key] = value;
  }

 // /**
 //   * Returns a combined array of id and value columns,
 //   *whereas each column consists of key, title, hidden and
 //   * optionally align, comparator, formatter and sortable
 //   */
 // this.getTableColumns = function() {
 //   var table_columns = [];
 //   metric_cfg.id_columns.forEach(function(c) {
 //     var hidden = true;
 //     cfg.id_columns.forEach(function(cc) {
 //       if (c.id == cc.id) {
 //         hidden = false;
 //         return;
 //       }
 //     });

 //     table_columns.push({
 //       key: c.id,
 //       title: c.name,
 //       hidden: hidden,
 //       sortable: c.sortable
 //     });
 //   });

 //   return table_columns.concat(this.getValueTableColumns());
 // };

 // /**
 //   * Returns an array of value columns, whereas each column consists of key,
 //   * title, hidden and optionally align, comparator, formatter and sortable
 //   */
 // this.getValueTableColumns = function() {
 //   var table_columns = [];
 //   metric_cfg.value_columns.forEach(function(c) {
 //     var hidden = true;
 //     cfg.value_columns.forEach(function(cc) {
 //       if (c.id == cc.id) {
 //         hidden = false;
 //         return;
 //       }
 //     });

 //     var col = {
 //       key: c.id,
 //       title: c.name,
 //       hidden: hidden
 //     };

 //     for (var prop in c) {
 //       if (prop == "id" || prop == "name") {
 //         continue;
 //       }

 //       col[prop] = c[prop]
 //     }

 //     table_columns.push(col);
 //   });

 //   return table_columns;
 // }

 // this.removeColumn = function(col_type, col_id) {
 //   if (!cfg.hasOwnProperty(col_type)) {
 //     return;
 //   }

 //   for (var i = 0; i < cfg[col_type].length; i++) {
 //     if (cfg[col_type][i].id == col_id) {
 //       cfg[col_type].splice(i, 1);
 //       return;
 //     }
 //   }
 // }

 // this.addColumn = function(col_type, col_id) {
 //   if (!cfg.hasOwnProperty(col_type) || !metric_cfg.hasOwnProperty(col_type)) {
 //     return;
 //   }

 //   for (var i = 0; i < cfg[col_type].length; i++) {
 //     if (cfg[col_type][i].id == col_id) {
 //       return;
 //     }
 //   }

 //   for (var i = 0; i < metric_cfg[col_type].length; i++) {
 //     if (metric_cfg[col_type][i].id == col_id) {
 //       cfg[col_type].push(metric_cfg[col_type][i]);
 //       return;
 //     }
 //   }
 // }

  this.getParamList = function() {
    var param_cfg = {};

    if (cfg.hasOwnProperty("view")) {
      param_cfg.view = cfg.view;
    }

    if (cfg.hasOwnProperty("order_by")) {
      param_cfg.order_by = cfg.order_by;
    }

    if (cfg.hasOwnProperty("order")) {
      param_cfg.order = cfg.order;
    }

    if (cfg.hasOwnProperty("filter")) {
      param_cfg.filter = cfg.filter;
    }

    return {
      cfg: JSON.stringify(param_cfg),
      offset: cfg.offset
    };
  }

  var initialize = function() {
    if (uri_params.config) {
      cfg = JSON.parse(uri_params.config);
      if (uri_params.offset) {
        cfg.offset = uri_params.offset;
      }

    }

    if (!cfg.hasOwnProperty("offset")) {
      cfg.offset = default_offset;
    }

    if (!cfg.hasOwnProperty("limit")) {
      cfg.limit = default_limit;
    }

    if (!cfg.hasOwnProperty("order")) {
      cfg.order = default_order;
    }

    if (!cfg.hasOwnProperty("view")) {
      cfg.view = default_view;
    }
  }

  initialize();
};


