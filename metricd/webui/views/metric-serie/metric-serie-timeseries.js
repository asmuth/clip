FnordMetric.views["fnordmetric.metric.serie.timeseries"] = function(elem, params) {
  var table;

  this.initialize = function() {
    var page = templateUtil.getTemplate("fnordmetric-metric-timeseries-tpl");
    elem.appendChild(page);

    renderCharts(params.data);
  };

  var renderCharts = function(values) {
    var chart_canvas = elem.querySelector(".chart_canvas");
    var rows = values.map(function(x) {
      return [new Date(x[0] / 1000), x[1]];
    });

    new Dygraph(
        chart_canvas,
        rows,
        {
          labels: ["time", "value"],
          colors: ["#3491DF", "#99C8E2"],
          gridLineColor: "#e6e6e6",
          axisLineColor: "#e6e6e6",
          axes: {
            x: {
              axisLabelFormatter: function(d, gran, opts) {
                console.log(gran, opts);
                return dateUtil.formatDateTime(d.getTime());
              }
            }
          }
          //series: {
          //  test: { axis: "y2"},
          //  mem_used: { axis: "y2"}
          //},
          //axes: {
          //  y: {
          //    independentTicks: true
          //  },
          //  y2: {
          //    drawGrid: true,
          //    independentTicks: true
          //  },
          //  x: {
          //    independentTicks: false
          //  }
          //}
        });

  }
}

