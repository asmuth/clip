FnordMetric.js_api = (function(){

  var timer = null;
  var update_interval = 1000;

  var widgets = [];

  function init() {
    $("div[data-fnordmetric]").each(function(n, e){
      var elem = $(e);

      if (elem.attr("data-fnordmetric") == "timeseries") {
        var widget = FnordMetric.widgets.timeseriesWidget();
        var gauges = elem.attr("data-gauges").split(",");
        var colors = elem.attr("data-colors");
        if (colors) { colors = colors.split(",") } else { colors = []; }
        var series = [];

        var wstyle = elem.attr("data-style");
        if (!wstyle) { wstlye = "line"; }

        var wheight = elem.attr("data-height");
        if (!wheight) { wheight = 240; }
        wheight = parseInt(wheight, 10);

        var wupdate = elem.attr("data-autoupdate");
        if (!wupdate) { wupdate = 60; }
        wupdate = parseInt(wupdate, 10);

        var wrange = elem.attr("data-time-range");
        if (!wrange) { wrange = 3600; }
        wrange = parseInt(wrange, 10);

        $(gauges).each(function(i, gname){
          var gcolor = colors[i];
          if (!gcolor) { gcolor = "#4572a7"; }
          series.push(
            { "color": gcolor, "data": [{x:0, y:0}], name: gname });
        });

        wkey = FnordMetric.util.getNextWidgetUID();

        widget.render({
          elem: elem,
          async_chart: true,
          include_current: true,
          width: 100,
          height: wheight,
          ext: true,
          gauges: gauges,
          series: series,
          default_cardinal: false,
          default_style: wstyle,
          autoupdate: wupdate,
          trange: wrange,
          widget_key: wkey
        });

        widgets.push(widget);
      }

    });
  }

  function socketOpen(){
    console.log("[FnordMetric] connected...");
    init();
  }

  function socketClose(){
    console.log("[FnordMetric] socket closed");
    window.setTimeout(FnordMetric.connect, 1000);
  }

  function socketMessage(raw){
    var evt = JSON.parse(raw.data);

    for (n = 0; n < widgets.length; n++)
      widgets[n].announce(evt);
  }

  return {
    init: init,
    socketOpen: socketOpen,
    socketClose: socketClose,
    socketMessage: socketMessage
  };

}());
