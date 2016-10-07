FnordMetric.views["fnordmetric.metric.timeseries"] = function(elem, params) {
  var table;

  this.initialize = function() {
    var page = zTemplateUtil.getTemplate("fnordmetric-metric-timeseries-tpl");
    elem.appendChild(page);

    renderCharts(params.data);
  };

  var renderCharts = function(results) {
    var chart_container = elem.querySelector(".timeseries");
    var chart_tpl = zTemplateUtil.getTemplate(
        "fnordmetric-metric-timeseries-chart-tpl").querySelector(".chart_box");

    var chart_boxes = [];
    results.forEach(function(r) {
      var chart_box = chart_tpl.cloneNode(true);
      chart_container.appendChild(chart_box);
      chart_boxes.push(chart_box);
    });

    console.log(results.length);

    for (var j = 0; j < results.length; j++) {
      var result = results[j];
      var time_col_idx;
      for (var i = 0; i < result.columns.length; i++) {
        if (result.columns[i] == "time") {
          time_col_idx = i;
          break;
        }
      }

      if (typeof time_col_idx !== "number") {
        console.log("ERROR: no time column", result.columns);
        return;
      }

      var rows = [];
      result.rows.forEach(function(r) {
        var row = [];
        for (var i = 0; i < r.length; i++) {
          if (i == time_col_idx) {
            row.push(new Date(r[i]));
          } else {
            row.push(r[i]);

            var test = r[i] + i * 100
            row.push(test);
          }
        }

        rows.push(row);
      });

      var columns = result.columns;
      columns.push("test");
      console.log(columns);


      new Dygraph(
          chart_boxes[j].querySelector(".chart_canvas"),
          rows,
          {
            labels: result.columns,
            title: result.title,
            colors: ["#3491DF", "#99C8E2"],
            gridLineColor: "#e6e6e6",
            axisLineColor: "#e6e6e6",
            series: {
              test: { axis: "y2"},
              mem_used: { axis: "y2"}
            },
            axes: {
              y: {
                independentTicks: false
              },
              y2: {
                drawGrid: true,
                independentTicks: true
              },
              x: {
                independentTicks: false
              }
            }
          });
    }

  }
  
}

