FnordMetric.views["fnordmetric.metric"] = function(elem, params) {
  var path = params.path;
  var url_params;
  var view_cfg;

  this.initialize = function() {
    url_params = getParams(params.path);
    var page = zTemplateUtil.getTemplate("fnordmetric-metric-detail-tpl");

    zDomUtil.handleLinks(page, params.app.navigateTo);
    zDomUtil.replaceContent(elem, page);
    viewport = new zViewport(elem.querySelector(".view_content"));

    loadMetricConfig(function(metric_cfg) {
      view_cfg = new FnordMetric.MetricTableViewConfig(metric_cfg, url_params);
      render();
      fetchData();
    });
  }

  var updatePath = function() {
    params.app.navigateTo(
        params.route.args[0] + "?" + zURLUtil.buildQueryString(view_cfg.getParamList()));
  }

  var getParams = function() {
    var p = {};

    p.metric = params.route.args[1];
    var cfg_param = zURLUtil.getParamValue(path, "cfg");
    if (cfg_param) {
      p.config = cfg_param;
    }

    var offset_param = zURLUtil.getParamValue(path, "offset");
    if (offset_param) {
      p.offset = offset_param;
    }

    return p;
  }

  var loadMetricConfig = function(callback) {
    //REMOVE ME
    var metric_cfg = {
      id_columns: [
        { "id": "time", "name": "Time", "sortable": true, "type": "string" },
        { "id": "sensor", "name": "Host", "sortable": true, "type": "string" },
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

    callback(metric_cfg);
  };

  var fetchData = function() {
    var query = view_cfg.getQuery();
    //TODO view_cfg.getQuery(), sendRequest

    //REMOVE ME
    var results = [];
    if (view_cfg.getValue("view") == "table") {
      results.push({
        columns: ["sensor", "datacenter", "cpu_time", "mem_used", "mem_free", "uptime"],
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
      });
    } else {
      for (var i = 0; i < 5; i++) {
        results.push({
          metric: "mem_used",
          columns: ["time", "mem_used"],
          rows: [
            [1472132898544, 1233],
            [1472132909164, 1635],
            [1472132922054, 3125],
            [1472132930592, 4222],
            [1472132938844, 3346],
            [1472132946423, 2023],
            [1472132954422, 1334],
            [1472132965900, 923]
          ]
        });
      }
    }
    //REMOVEME END

    //updateTable(result);
    //renderPagination(result.rows.length);
    renderView(results);
  }

  var render = function() {
    renderHeader();
    // renderView(); --> initView
    renderFilterList();
    renderEmbedControl();
  };

  var renderHeader = function(metric) {
    var header = elem.querySelector(
        ".fnordmetric-metric-table .page_header .metric_name");
    header.innerHTML = url_params.metric;

    /* handle view controls */
    elem.querySelector(".fnordmetric-metric-table .view_control").setAttribute(
        "data-view", view_cfg.getValue("view"));

    var table_view_ctrl = elem.querySelector(
        ".fnordmetric-metric-table .view_control .table_view");
    table_view_ctrl.addEventListener("click", function(e) {
      view_cfg.updateValue("view", "table");
      updatePath();
    }, false);

    var timeseries_view_ctrl = elem.querySelector(
        ".fnordmetric-metric-table .view_control .timeseries_view");
    timeseries_view_ctrl.addEventListener("click", function(e) {
      view_cfg.updateValue("view", "timeseries");
      updatePath();
    }, false);
  };

  var renderView = function(results) {
    //TODO enable / disable current view icon in header
    var view;
    var view_opts = {
      data: results,
      view_cfg: view_cfg
    };

    switch (view_cfg.getValue("view")){
      case "table":
        view = FnordMetric.views["fnordmetric.metric.table"];
        break;

      case "timeseries":
        view = FnordMetric.views["fnordmetric.metric.timeseries"];
        break;

      default:
        view = FnordMetric.views["fnordmetric.404"];
        break;
    }

    viewport.setView(view, view_opts);
  }

  var renderFilterList = function() {
    var filter_strs = view_cfg.getValue("filter") ? view_cfg.getValue("filter") : [];
    var filter_ctrl = new FnordMetric.MetricTableFilter(
        elem.querySelector(".fnordmetric-metric-table z-modal.filter"));

    /* add a new filter */
    var add_link = elem.querySelector(
        ".fnordmetric-metric-table .sidebar a.add_filter");
    add_link.addEventListener("click", function(e) {
      filter_ctrl.render();
    }, false);
    filter_ctrl.onSubmit(function(filter_str) {
      console.log(filter_str);
      filter_strs.push(filter_str);
      view_cfg.updateValue("filter", filter_strs);
      updatePath();
    });

    /* render exisiting filter list */
    var flist_elem = elem.querySelector(
        ".fnordmetric-metric-table .sidebar .filter_list");
    var felem_tpl = zTemplateUtil.getTemplate(
        "fnordmetric-metric-table-filter-list-elem-tpl");

    filter_strs.forEach(function(f) {
      var felem = felem_tpl.cloneNode(true);
      felem.querySelector(".filter_value").innerHTML = f;

      felem.querySelector(".edit").addEventListener("click", function(e) {
        filter_ctrl.render(f);
      });

      flist_elem.appendChild(felem);
    });

    /* change an existing filter */
    filter_ctrl.onChange(function(new_filter_str, old_filter_str) {
      var idx = filter_strs.indexOf(old_filter_str);
      if (idx > -1) {
        filter_strs[idx] = new_filter_str;
      }

      view_cfg.updateValue("filter", filter_strs);
      updatePath();
    });

    /* remove an exisiting filter */
    filter_ctrl.onDelete(function(old_filter_str) {
      var idx = filter_strs.indexOf(old_filter_str);
      if (idx > -1) {
        filter_strs.splice(idx, 1);
      }
      view_cfg.updateValue("filter", filter_strs);
      updatePath();
    });
  }

  var renderEmbedControl = function() {
    elem.querySelector(".fnordmetric-metric-table .control.embed")
        .addEventListener("click", function() {
          fEmbedPopup(elem, "").render();
        }, false);
  }
}

