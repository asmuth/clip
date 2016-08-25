FnordMetric.views["fnordmetric.metric.sensor"] = function(elem, params) {
  var url_params = {};
  var base_url = "/metrics";

  this.initialize = function() {
    url_params = getUrlParams();

    var page = zTemplateUtil.getTemplate("fnordmetric-metric-sensor-tpl");
    renderHeader(page);

    zDomUtil.handleLinks(page, params.app.navigateTo);
    elem.appendChild(page);

    loadMetricConfig(function(metric_cfg) {
      renderLoadingCharts(metric_cfg);
      fetchData();
    });
  };

  this.destroy = function() {

  };

  var getUrlParams = function() {
    var p = {};

    p.metric = params.route.args[1];
    p.sensor = params.route.args[2];

    return p;
  }

  var loadMetricConfig = function(callback) {
    //REMOVE ME
    var metric_cfg = {
      id_columns: [
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
  }

  var fetchData = function() {
    //REMOVE ME
    var result = {
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
    };
    //REMOVE ME END
  }

  var renderHeader = function(page) {
    var metric_link = page.querySelector(".page_header .metric_name");
    metric_link.setAttribute(
        "href",
        zURLUtil.joinPaths(base_url, url_params.metric));
    metric_link.innerHTML = url_params.metric;

    page.querySelector(".page_header .sensor_name").innerHTML = url_params.sensor;
  }

  var renderLoadingCharts = function(metric_cfg) {
    var chart_box = elem.querySelector(
        ".fnordmetric-metric-sensor .chart_box_container");
    var chart_tpl = zTemplateUtil.getTemplate(
        "fnordmetric-metric-sensor-chart-tpl").querySelector(".chart_box");

    zDomUtil.clearChildren(chart_box);
    metric_cfg.value_columns.forEach(function(c) {
      var html = chart_tpl.cloneNode(true);
      html.querySelector(".chart_title").innerHTML = c.name;
      chart_box.appendChild(html);
    });
  }

}
