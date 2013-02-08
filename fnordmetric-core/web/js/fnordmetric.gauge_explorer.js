FnordMetric.views.gaugeExplorer = (function(){

  var elem, widget, gauge_list;
  var currentGauge;

  function load(_elem){
    elem = _elem;

    elem.html('<div class="navbar"></div><div class="ge_controlpanel">' +
     '<label>Select a Gauge</label><select class="ge_gauge_picker"></select>' +
     '</div><div class="widget_viewport"></div>');

    FnordMetric.ui.navbar($('.navbar', elem), {
      breadcrumb: [
        ["Gauge Explorer", "#gauge_explorer"],
      ],
      buttons: [
        ["<i class='icon-refresh'></i>Refresh", function(){ load(elem); }]
      ],
      datepicker: true
    });

    currentGauge = {
      key: "useronline_total",
      title: "Fnord"
    };

    render();
    requestGaugeInfoAsync();
  };

  function render() {
    console.log(currentGauge);
    $(".widget_viewport", elem).html('');
    widget = FnordMetric.widgets.timeseriesWidget();

    var gauges = [currentGauge.key];
    var series = [{
      "color": "#00ff00",
      "data": [{x:0, y:0}],
      name: currentGauge.key,
      title: currentGauge.title
    }];

    widget.render({
      elem: $(".widget_viewport", elem),
      async_chart: true,
      start_timestamp: ((new Date()).getTime() - 3600),
      end_timestamp: ((new Date()).getTime()),
      include_current: true,
      width: 100,
      height: (window.innerHeight - 400),
      gauges: gauges,
      series: series,
      default_cardinal: false,
      default_style: "line",
      autoupdate: 10,
      tick: 10,
      widget_key: "gauge_explorer"
    });
  }

  function resize(_width, _height){
    render();
  };

  function announce(evt){
    if (evt.type == "widget_response" && widget)
      widget.announce(evt);

    if (evt.type == "gauge_list_response"){
      $('.ge_gauge_picker', elem).html();
      gauge_list = {};

      $(evt.gauges).each(function(n, gauge){
        var gauge_title = gauge.title + ' [' + gauge.key + ']';

        gauge_list[gauge.key] = {
          "key": gauge.key, 
          "title": gauge.title
        };

        $('.ge_gauge_picker', elem).append(
          '<option value=' + gauge.key + '>' + gauge_title + '</option>');
      });

      $('.ge_gauge_picker', elem)
        .bind('value_changed', function(){
          currentGauge = gauge_list[$(this).val()];
          render();
        })
        .combobox();
    }
  }

  function requestGaugeInfoAsync() {
    FnordMetric.publish({
      "type": "gauge_list_request"
    });
  }

  function close(){
  };

  return {
    load: load,
    resize: resize,
    announce: announce,
    close: close
  };

});
