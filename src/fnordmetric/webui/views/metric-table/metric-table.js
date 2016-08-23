FnordMetric.views["fnordmetric.metric.table"] = function(elem, params) {

  var ViewConfig = function(metric_cfg, uri_params) {
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
      if (!cfg.hasOwnProperty(col_type) || metric_cfg.hasOwnProperty(col_type)) {
        return;
      }

      for (var i = 0; i < cfg[col_type].length; i++) {
        if (cfg[col_type][i].id == col_id) {
          return;
        }
      }

      for (var i = 0; i < metric_cfgg[col_type].length; i++) {
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

  var url_params = {};
  var table = new zTable({});

  var view_cfg;
  var metric_cfg;

  this.initialize = function() {
    url_params = getUrlParams(params.path);

    var page = zTemplateUtil.getTemplate("fnordmetric-metric-table-tpl");

    zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(page);

    table.onClick(function(r) {
      params.app.navigateTo("/metrics/" + r.metric_id);
    });

    loadMetricConfig();
  };

  this.destroy = function() {
    //query_mgr.closeAll();
  };

  var getUrlParams = function(path) {
    var p = {};

    p.metric = params.route.args[1];
    var cfg_param = zURLUtil.getParamValue(path, "cfg");
    if (cfg_param) {
      p.config = cfg_param;
    }

    return p;
  };

  var loadMetricConfig = function() {
    //REMOVE ME
    metric_cfg = {
      id_columns: [
        { "id": "host", "name": "Host" },
        { "id": "datacenter", "name": "Datacenter" },
      ],
      value_columns: [
        { "id": "cpu_time", "name": "CPU Time" },
        { "id": "mem_used", "name": "Memory used" },
        { "id": "mem_free", "name": "Memory free" },
        { "id": "uptime", "name": "Uptime" },
        { "id": "load_avg", "name": "Load Average" },
        { "id": "processes", "name": "Processes" },
        { "id": "kernel_state", "name": "Kernel state" }
      ]
    };
    //REMOVE ME END

    view_cfg = new ViewConfig(metric_cfg, url_params);
    render();
  };

  var update = function() {
    params.app.navigateTo(
        params.path + "?" + zURLUtil.buildQueryString(view_cfg.getParamList()));
  }

  var render = function() {
    renderHeader();
    renderTable();
    renderValueColumsControl();
  };

  var renderHeader = function(metric) {
    var header = elem.querySelector(
        ".fnordmetric-metric-table .page_header .metric_name");
    header.innerHTML = url_params.metric;
  };

  var renderTable = function() {
    elem.querySelector(".fnordmetric-metric-table .metric-table")
        .innerHTML = JSON.stringify(view_cfg.get());
  };

  var renderValueColumsControl = function() {
    var ul = elem.querySelector(
        ".fnordmetric-metric-table .control_box .value_columns");

    var displayed_columns = view_cfg.getValue("value_columns");

    metric_cfg.value_columns.forEach(function(col) {
      var displayed = false;
      displayed_columns.forEach(function(c) {
        if (c.id == col.id) {
          displayed = true;
          return;
        }
      });

      var li = document.createElement("li");
      li.innerHTML = col.name;
      var icon = document.createElement("i");
      icon.className = "fa";
      li.className = displayed ? "selected" : "";

      li.appendChild(icon);
      ul.appendChild(li);

      icon.addEventListener("click", function(e) {
        if (displayed) {
          view_cfg.removeColumn("value_columns", col.id);
        } else {
          view_cfg.addColumn("value_columns", col.id);
        }
        update();
      });

      li.addEventListener("mouseover", function(e) {
        //console.log("highlight " + col.id + " column");
      });

      li.addEventListener("mouseout", function(e) {
        //console.log("remove highlight " + col.id + " column");
      });
    });
  }
}
