if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.widgets == 'undefined')
  FnordMetric.widgets = {};

FnordMetric.widgets.timeseries = function(elem){
  var graph, gauges, colors, gconfig, legend, hoverDetail, shelving,
      highlighter, refresh_timer, series, height, display_legend,
      widget_key;

  var default_colors = ["#db843d", "#3d96ae", "#80699b", "#89a54e",
                         "#aa4643", "#4572a7"]

  function init() {
    widget_key = elem.attr("data-widget-key");

    if (elem.attr('data-legend') == "off")
      display_legend = false;
    else
      display_legend = true;

    renderLayout();

    if (!elem.attr('data-gauges'))
      return console.log("[FnordMetric] element is missing the data-gauges attribute");

    gauges = elem.attr("data-gauges").split(",");

    if (elem.attr('data-colors'))
      colors = elem.attr("data-colors").split(",");
    else
      colors = [];

    for (n = 0; n < gauges.length; n++)
      if (!colors[n]) colors[n] = default_colors.pop();

    gconfig = {
      element: $('.fnordmetric_container', elem)[0],
      padding: { top: 0.1, bottom: 0 },
      stroke: true
    };

    if (elem.attr('data-cardinal') == "on") {
      gconfig.interpolation = 'cardinal';
    } else {
      gconfig.interpolation = 'linear';
    }

    if (elem.attr('data-chart-style') == 'area') {
      gconfig.renderer = 'area';
      gconfig.offset = 'zero';
    }

    else if (elem.attr('data-chart-style') == 'flow') {
      gconfig.renderer = 'stack';
      gconfig.offset = 'silhouette';
    }

    else {
      gconfig.renderer = 'line';
      gconfig.offset = 'zero';
    }

    if (elem.attr('data-height'))
      height = parseInt(elem.attr('data-height'), 10);
    else
      height = 240;

    resize();
    requestDataAsync();

    if (refresh_interval = elem.attr('data-autoupdate'))
      refresh_interval = parseFloat(refresh_interval);

    if (refresh_interval)
      refresh_timer = window.setInterval(
        requestDataAsync, refresh_interval * 1000);
  }

  function renderLayout() {
    if (display_legend)
      $(elem).append(
        $('<div></div>')
          .addClass('fnordmetric_container_legend')
          .css({
            margin: '10px 30px 0 30px',
          })
      );

    $(elem).append(
      $('<div></div>')
        .addClass('fnordmetric_container')
        .css({
          height: height,
          margin: '0 23px 25px 23px',
        })
    );
  }

  function renderChart() {
    $(gconfig.element).html("");

    if (display_legend)
      $(".fnordmetric_legend", elem).html("");

    graph = new FnordMetric.rickshaw.Graph(gconfig);

    if (display_legend)
      legend = new FnordMetric.rickshaw.Graph.Legend({
        graph: graph,
        element: $('.fnordmetric_container_legend', elem)[0]
      });

    hoverDetail = new FnordMetric.rickshaw.Graph.HoverDetail( {
      graph: graph
    });

    shelving = new FnordMetric.rickshaw.Graph.Behavior.Series.Toggle({
      graph: graph,
      legend: legend
    });

    highlighter = new FnordMetric.rickshaw.Graph.Behavior.Series.Highlight({
      graph: graph,
      legend: legend
    });

    new FnordMetric.rickshaw.Graph.Axis.Time({
      graph: graph,
    }).render();

    new FnordMetric.rickshaw.Graph.Axis.Y({
      graph: graph,
    }).render();

    if(!gconfig.renderer){
      gconfig.renderer = "line";
    }

    graph.configure(gconfig);
    graph.render();
  }

  function resize() {
    gconfig.width = elem.width() - 50;
    gconfig.height = height;
  }

  function send(evt) {
    var dgauges = evt.gauges;
    gconfig.series = [];

    for(var ind = 0; ind < dgauges.length; ind++){

      gconfig.series.push({
        name: dgauges[ind].title,
        color: colors[ind],
        data: []
      });

      for(_time in dgauges[ind].vals){
        gconfig.series[ind].data.push(
          { x: parseInt(_time), y: parseInt(dgauges[ind].vals[_time] || 0) }
        );
      }

    }

    renderChart();
  }

  function requestDataAsync() {
    FnordMetric.publish({
      "type": "widget_request",
      "klass": "generic",
      "gauges": gauges,
      "cmd": "values_at",
      "tick": 30,
      "since": (((new Date()).getTime() / 1000)- 3600),
      "until": ((new Date()).getTime() / 1000),
      "widget_key": widget_key
    });
  }

  function destroy() {
    if (refresh_timer)
      window.clearInterval(refresh_timer);
  }


  return {
    init: init,
    send: send,
    destroy: destroy
  };

};
