FnordMetric.views["fnordmetric.metric.table"] = function(elem, params) {

  var ViewConfig = function(metric_cfg, uri_params) {
    var cfg = {
      id_columns: [],
      value_columns: []
    }

    this.update = function(property, value) {

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

    this.getQueryString = function() {

    }

    var initialize = function() {
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
      cfg.limit = 25;
      cfg.order = cfg.value_columns[0].id;
    }

    initialize();
  };

  var url_params = {};
  var table = new zTable({});

  //info about the current view
  var view_cfg = {};

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
    var metric_cfg = {
      id_columns: [
        { "id": "host", "name": "Host" },
        { "id": "datacenter", "name": "Datacenter" },
      ],
      value_columns: [
        { "id": "cpu_time", "name": "CPU Time" },
        { "id": "mem_used", "name": "Memory used" },
        { "id": "mem_free", "name": "Memory free" },
        { "id": "uptime", "name": "Uptime" },
        { "id": "load_avg", "name": "Load Average" }
      ]
    };
    //REMOVE ME END

    view_cfg = new ViewConfig(metric_cfg, url_params);
    render();
  };

  var updateViewConfig = function() {

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
    displayed_columns.forEach(function(col) {
      var li = document.createElement("li");
      li.innerHTML = col.name;
      var display_icon = document.createElement("i");
      display_icon.className = "fa fa-eye";

      li.appendChild(display_icon);
      ul.appendChild(li);

      display_icon.addEventListener("click", function(e) {
        
      });

      li.addEventListener("mouseover", function(e) {
        console.log("highlight " + col.id + " column");
      });
      li.addEventListener("mouseout", function(e) {
        console.log("remove highlight " + col.id + " column");
      });
    });
  }
}
