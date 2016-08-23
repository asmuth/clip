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
        { "id": "host" },
        { "id": "datacenter" },
      ],
      value_columns: [
        { "id": "cpu_time" },
        { "id": "mem_used" },
        { "id": "mem_free" },
        { "id": "uptime" },
        { "id": "load_avg" }
      ]
    };
    //REMOVE ME END

    view_cfg = new ViewConfig(metric_cfg, url_params);
    renderTable();
  };

  var updateViewConfig = function() {

  }

  var renderHeader = function(metric) {
    var header = elem.querySelector(
        ".fnordmetric-metric-table .page_header .metric_name");
    header.innerHTML = metric;
  };

  var renderTable = function() {
    elem.querySelector(".fnordmetric-metric-table .metric-table")
        .innerHTML = JSON.stringify(view_cfg.get());
  };
}
