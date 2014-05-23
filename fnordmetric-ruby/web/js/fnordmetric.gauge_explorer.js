FnordMetric.views.gaugeExplorer = (function(){

  var elem, widget, gauge_list, widget_inst;
  var start_timestamp, end_timestamp, time_range;
  var currentGauge;
  var autoupdate_timer, autodiscover_timer;

  var time_range = 3600;
  var default_time_ranges = [
    ["5m", 300],
    ["15m", 900],
    ["30m", 1800],
    ["1h", 3600],
    ["2h", 3600 * 2],
    ["6h", 3600 * 6],
    ["12h", 3600 * 12],
    ["24h", 3600 * 24],
    ["48h", 3600 * 48]
  ];

  function load(_elem){
    elem = _elem;
    gauge_list = {};

    elem.html('<div class="navbar"></div><div class="ge_controlpanel">' +
       '<label>Select a Gauge</label><select class="ge_gauge_picker">' + 
       '<option selected=selected>Please select a gauge...</option></select>' +
     '<div class="ge_meta">Type to autocomplete...</div>' +
     '</div><div class="ge_controlpanel" style="border-right:none;">' +
       '<label>Select the Timerange</label><a class="button datepicker_sa"><div class="date" style="width:300px;">&nbsp;</div>' +
       '<i class="icon-calendar"></i></a>' +
     '<div class="ge_timerange_links ge_meta">Recent Data: </div></div>' +
     '<div style="clear:both;" class="widget_viewport"><div class="headbar"><h2>Please select a gauge...</h2></div><i class="ge_empty">Please select a gauge...</i></div>');

    FnordMetric.ui.navbar($('.navbar', elem), {
      breadcrumb: [
        ["Gauge Explorer", "#gauge_explorer"],
      ],
      buttons: [
        ["<i class='icon-refresh'></i>Refresh", function(){ load(elem); }]
      ]
    });

    $(default_time_ranges).each(function(i, tr){
      $('.ge_timerange_links', elem).append(
        $('<a>')
          .attr('href', '#')
          .attr('data-timerange', tr[1])
          .click(function(){
            time_range = parseInt($(this).attr('data-timerange'), 10);
            autoupdate();
            return false; 
          })
          .html(tr[0]));
    });

    autoupdate();
    render();
    requestGaugeInfoAsync();

    autodiscover_timer = window.setInterval(requestGaugeInfoAsync, 5000);
    autoupdate_timer = window.setInterval(autoupdate, 1000);
  };

  function autoupdate() {
    end_timestamp = ((new Date()).getTime() / 1000) + 60;
    start_timestamp = end_timestamp - time_range;

    if (widget)
      widget.set_timerange(start_timestamp, end_timestamp);

    $(".datepicker_sa .date", elem).html(
      FnordMetric.util.dateFormat(start_timestamp) +
      '&nbsp;&dash;&nbsp;' +
      FnordMetric.util.dateFormat(end_timestamp)
    )
  }

  function render() {
    if (!currentGauge) return;

    $(".widget_viewport", elem).html('');
    widget = FnordMetric.widgets.timeseriesWidget();

    var gauges = [currentGauge.key];
    var series = [{
      "color": "#4572a7",
      "data": [{x:0, y:0}],
      name: currentGauge.key,
      title: currentGauge.title
    }];

    var wheight = FnordMetric.get_conf().gauge_explorer_widget_height;

    if (!wheight)
      wheight = window.innerHeight - 300;

    widget_inst = widget.render({
      elem: $(".widget_viewport", elem),
      title: currentGauge.title,
      async_chart: true,
      start_timestamp: start_timestamp,
      end_timestamp: end_timestamp,
      include_current: true,
      width: 100,
      height: wheight,
      gauges: gauges,
      series: series,
      default_cardinal: false,
      default_style: "line",
      no_update_range: true,
      no_datepicker: true,
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
      var changed = false;

      $(evt.gauges).each(function(n, gauge){
        if (typeof gauge_list[gauge.key] == 'undefined') {
          var gauge_title = gauge.title + ' [' + gauge.key + ']';

          gauge_list[gauge.key] = {
            "key": gauge.key, 
            "title": gauge.title
          };

          $('.ge_gauge_picker', elem).append(
            '<option value=' + gauge.key + '>' + gauge_title + '</option>');

          changed = true;
        }
      });

      if (changed || evt.gauges.length == 0)
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
    window.clearInterval(autoupdate_timer);
    window.clearInterval(autodiscover_timer);
  };

  return {
    load: load,
    resize: resize,
    announce: announce,
    close: close
  };

});
