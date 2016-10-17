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


