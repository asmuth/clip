FnordMetric.views["fnordmetric.metric.table"] = function(elem, params) {

  var url_params = {};
  var table;

  var view_cfg;
  var metric_cfg;

  this.initialize = function() {
    url_params = getUrlParams(params.path);

    var page = zTemplateUtil.getTemplate("fnordmetric-metric-table-tpl");

    zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(page);

    loadMetricConfig(function() {
      view_cfg = new FnordMetric.MetricTableViewConfig(metric_cfg, url_params);
      render();
      fetchData();
    });
  };

  this.destroy = function() {
    //query_mgr.closeAll();
  };

  this.changePath = function(new_path) {
    //TODO update view, fetch data
  }

  var getUrlParams = function(path) {
    var p = {};

    p.metric = params.route.args[1];
    var cfg_param = zURLUtil.getParamValue(path, "cfg");
    if (cfg_param) {
      p.config = cfg_param;
    }

    return p;
  };

  var loadMetricConfig = function(callback) {
    //REMOVE ME
    metric_cfg = {
      id_columns: [
        { "id": "host", "name": "Host", "sortable": true, "type": "string" },
        { "id": "datacenter", "name": "Datacenter", "sortable": true, "type": "string" },
      ],
      value_columns: [
        { "id": "cpu_time", "name": "CPU Time", "sortable": true, "type": "num" },
        { "id": "mem_used", "name": "Memory used", "sortable": true, "type": "num" },
        { "id": "mem_free", "name": "Memory free", "sortable": true, "type": "num" },
        { "id": "uptime", "name": "Uptime", "sortable": true, "type": "num" },
        { "id": "load_avg", "name": "Load Average", "sortable": true, "type": "num" },
        { "id": "processes", "name": "Processes", "sortable": true, "type": "num" },
        { "id": "kernel_state", "name": "Kernel state", "sortable": true, "type": "num" }
      ]
    };
    //REMOVE ME END

    callback();
  };

  var fetchData = function() {
    var query = view_cfg.getQuery();
    //TODO view_cfg.getQuery(), sendRequest

    //REMOVE ME
    var result = {
      columns: ["host", "datacenter", "cpu_time", "mem_used", "mem_free", "uptime"],
      rows: [
        ["nue01", "nue", "14355625767", "104", "7", "233445"],
        ["nue02", "nue", "14355625767", "104", "7", "233445"],
        ["nue03", "nue", "14355625767", "104", "7", "233445"],
        ["nue04", "nue", "14355625767", "104", "7", "233445"],
        ["nue05", "nue", "14355625767", "104", "7", "233445"],
        ["nue06", "nue", "14355625767", "104", "7", "233445"],
        ["nue07", "nue", "14355625767", "104", "7", "233445"],
        ["nue08", "nue", "14355625767", "104", "7", "233445"],
        ["nue09", "nue", "14355625767", "104", "7", "233445"],
        ["nue10", "nue", "14355625767", "104", "7", "233445"],
        ["nue11", "nue", "14355625767", "104", "7", "233445"],
        ["nue12", "nue", "14355625767", "104", "7", "233445"],
      ]
    };
    //REMOVEME END

    updateTable(result);
  }

  var updatePath = function() {
    params.app.navigateTo(
        params.route.args[0] + "?" + zURLUtil.buildQueryString(view_cfg.getParamList()));
  }

  var render = function() {
    renderHeader();
    renderTable();
    renderValueColumsControl();
    renderFilterControl();
  };

  var renderHeader = function(metric) {
    var header = elem.querySelector(
        ".fnordmetric-metric-table .page_header .metric_name");
    header.innerHTML = url_params.metric;
  };

  var updateTable = function(update) {
    table.setRows(fTableUtil.buildRows(update.columns, update.rows));
    renderTable();
  }

  var renderTable = function() {
    //initialize table
    if (!table) {
      table = new zTable({columns: view_cfg.getTableColumns()});

      /* navigate to id detail page */
      table.onClick(function(r) {
        params.app.navigateTo("/metrics/" + r.metric_id);
      });

      /* sort callback */
      table.onSort(function(column, direction) {
        view_cfg.updateValue("order", direction);
        view_cfg.updateValue("order_by", column.key);
        updatePath();
      });
    }

    //TODO if update setVisibleColumns
    table.render(elem.querySelector(".fnordmetric-metric-table .metric-table"));
  };

  var renderValueColumsControl = function() {
    var value_columns = view_cfg.getValueTableColumns();
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
        var idx = columns.indexOf(c.key);
        if (c.hidden && idx > -1) {
          view_cfg.addColumn("value_columns", c.key);
        }

        if (!c.hidden && idx == -1) {
          view_cfg.removeColumn("value_columns", c.key);
        }
      });

      updatePath();
    });
  }

  var renderValueColumnsSidebar = function(value_columns) {
    var ul = elem.querySelector(
        ".fnordmetric-metric-table .sidebar .value_columns");
    value_columns.forEach(function(col) {
      var li = document.createElement("li");
      li.innerHTML = col.title;
      var icon = document.createElement("i");
      icon.className = "fa";
      li.className = col.hidden ? "" : "selected";

      li.appendChild(icon);
      ul.appendChild(li);

      icon.addEventListener("click", function(e) {
        if (col.hidden) {
          view_cfg.addColumn("value_columns", col.key);
        } else {
          view_cfg.removeColumn("value_columns", col.key);
        }
        updatePath();
      });

      if (!col.hidden) {
        li.addEventListener("mouseover", function(e) {
          table.highlightColumn(col.key);
        });

        li.addEventListener("mouseout", function(e) {
          table.unhiglightColumn();
        });
      }
    });
  }

  var renderFilterControl = function() {
    var filter = new FnordMetric.MetricTableFilter(
        elem.querySelector(".fnordmetric-metric-table z-modal.filter"));

    filter.onSubmit(function(filter_str) {
      view_cfg.updateValue("filter", filter_str);
      updatePath();
    });

    elem.querySelector(".fnordmetric-metric-table .control i.filter")
        .addEventListener("click", function(e) {
          filter.render(view_cfg.getValue("filter"));
        });
  }
}
