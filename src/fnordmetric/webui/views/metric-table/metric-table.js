FnordMetric.views["fnordmetric.metric.table"] = function(elem, params) {

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

    view_cfg = new FnordMetric.MetricTableViewConfig(metric_cfg, url_params);
    render();
  };

  var update = function() {
    params.app.navigateTo(
        params.route.args[0] + "?" + zURLUtil.buildQueryString(view_cfg.getParamList()));
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
    var displayed_columns = view_cfg.getValue("value_columns");
    var value_columns = [];
    metric_cfg.value_columns.forEach(function(c) {
      var hidden = true;
      displayed_columns.forEach(function(cc) {
        if (c.id == cc.id) {
          hidden = false;
          return;
        }
      });

      value_columns.push({
        id: c.id,
        name: c.name,
        hidden: hidden
      });
    });

    renderValueColumnsDropdown(value_columns);
    renderValueColumnsSidebar(value_columns);
  }

  var renderValueColumnsDropdown = function(value_columns) {
    var dropdown = elem.querySelector(
        ".fnordmetric-metric-table z-dropdown.value_columns");
    var selector = new fMetricSelector(dropdown, value_columns);
    selector.render();

    dropdown.addEventListener("change", function(e) {
      var columns = this.getValue().split(",");
      value_columns.forEach(function(c) {
        var idx = columns.indexOf(c.id);
        if (c.hidden && idx > -1) {
          view_cfg.addColumn("value_columns", c.id);
        }

        if (!c.hidden && idx == -1) {
          view_cfg.removeColumn("value_columns", c.id);
        }
      });

      update();
    });
  }

  var renderValueColumnsSidebar = function(value_columns) {
    var ul = elem.querySelector(
        ".fnordmetric-metric-table .sidebar .value_columns");
    value_columns.forEach(function(col) {
      var li = document.createElement("li");
      li.innerHTML = col.name;
      var icon = document.createElement("i");
      icon.className = "fa";
      li.className = col.hidden ? "" : "selected";

      li.appendChild(icon);
      ul.appendChild(li);

      icon.addEventListener("click", function(e) {
        if (col.hidden) {
          view_cfg.addColumn("value_columns", col.id);
        } else {
          view_cfg.removeColumn("value_columns", col.id);
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
