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
      renderLoadingCharts(metric_cfg); //race condition ?
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
    };

    //REMOVE ME END

    renderCharts([result]);
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
      html.setAttribute("data-metric", c.id);
      html.querySelector(".chart_title").innerHTML = c.name;
      chart_box.appendChild(html);
    });
  }

  var renderCharts = function(results) {
    results.forEach(function(r) {
      var time_col_idx;
      for (var i = 0; i < r.columns.length; i++) {
        if (r.columns[i] == "time") {
          time_col_idx = i;
          break;
        }
      }

      if (typeof time_col_idx !== "number") {
        console.log("ERROR: no time column", r.columns);
        return;
      }

      var rows = [];
      r.rows.forEach(function(r) {
        var row = [];
        for (var i = 0; i < r.length; i++) {
          if (i == time_col_idx) {
            row.push(new Date(r[i]));
          } else {
            row.push(r[i]);
          }
        }

        rows.push(row);
      });


      var chart_elem = elem.querySelector(
          ".fnordmetric-metric-sensor .chart_box[data-metric='" + r.metric + "']");
      new Dygraph(
          chart_elem.querySelector(".chart_canvas"),
          rows,
          {
            labels: r.columns,
            title: r.title,
            drawYGrid: true,
            drawXGrid: false
          });
    });
  }

}
