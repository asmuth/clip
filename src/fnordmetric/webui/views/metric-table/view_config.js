FnordMetric.MetricTableViewConfig = function(metric_cfg, uri_params) {
  var cfg = {
    id_columns: [],
    value_columns: []
  }

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

  this.removeColumn = function(col_type, col_id) {
    if (!cfg.hasOwnProperty(col_type)) {
      return;
    }

    for (var i = 0; i < cfg[col_type].length; i++) {
      if (cfg[col_type][i].id == col_id) {
        cfg[col_type].splice(i, 1);
        return;
      }
    }
  }

  this.addColumn = function(col_type, col_id) {
    if (!cfg.hasOwnProperty(col_type) || !metric_cfg.hasOwnProperty(col_type)) {
      return;
    }

    for (var i = 0; i < cfg[col_type].length; i++) {
      if (cfg[col_type][i].id == col_id) {
        return;
      }
    }

    for (var i = 0; i < metric_cfg[col_type].length; i++) {
      if (metric_cfg[col_type][i].id == col_id) {
        cfg[col_type].push(metric_cfg[col_type][i]);
        return;
      }
    }
  }

  this.getParamList = function() {
    var param_cfg = {
      value_columns: cfg.value_columns,
      id_columns: cfg.id_columns,
      order: cfg.order
    };

    var param_list = {
      cfg: JSON.stringify(param_cfg),
      offset: cfg.offset
    }

    return param_list;
  }

  var initialize = function() {
    if (uri_params.config) {
      cfg = JSON.parse(uri_params.config);
      if (uri_params.offset) {
        cfg.offset = uri_params.offset;
      }

    } else {
      //build default config
      var max_id_cols = 3;
      var max_value_cols = 4;

      if (metric_cfg.id_columns) {
        cfg.id_columns = metric_cfg.id_columns.slice(0, max_id_cols);

        // display more value columns if less than max id columns
        max_value_cols += max_id_cols - cfg.id_columns.length;
      }

      if (metric_cfg.id_columns) {
        cfg.value_columns = metric_cfg.value_columns.slice(0, max_value_cols);
      }

      cfg.offset = 0;
      cfg.order = cfg.value_columns[0].id;
    }

    cfg.limit = 25;
  }

  initialize();
};


