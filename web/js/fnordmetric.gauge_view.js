FnordMetric.views.gaugeView = (function(gauge_name){
  var widgets = [];
  var widget_objs = {};
  var viewport = null;
  var tabs = [];
  var conf = {};
  var tick;

  var start_timestamp, end_timestamp;

  function load(_viewport){
    viewport = _viewport;
    viewport.html('');
    viewport.append('<div class="navbar"></div>');
    viewport.append($('<div class="gauge_viewport"></div>'));

    conf.title = gauge_name;
    tick = FnordMetric.gauges[gauge_name].tick
    ctick = Math.max(3600, tick);

    FnordMetric.ui.navbar($('.navbar', viewport), {
      breadcrumb: [ 
        ["Fnord", "/group/fnord"],
        [conf.title, "/gauge/"+gauge_name]
      ],
      buttons: [
        ["<i class='icon-refresh'></i>Refresh", function(){ load(viewport); }]
      ],
      datepicker: true
    });

    var now = parseInt((new Date()).getTime() / 1000);

    start_timestamp = atTick((now-(ctick*24)));
    end_timestamp   = atTick(now);

    updateDatepicker();
    load_interval();
  }

  function atTick(t){
    return (Math.ceil(t / ctick) * ctick);
  }

  function close(){

  }

  function resize(){

  }

  function updateDatepicker(){
    $(".navbar .datepicker .date", viewport).html(
      FnordMetric.util.dateFormat(start_timestamp) +
      '&nbsp;&dash;&nbsp;' +
      FnordMetric.util.dateFormat(end_timestamp)
    ).parent().click(function(){
      open_interval_modal();
    });
  }

  function announce(evt){
    if ((evt.gauge != gauge_name) ||
       (evt.type != "render_response") ||
       (!evt.payload)){ return true; }

    if(evt.payload.html){
      $('.gauge_viewport', viewport)
        .html(evt.payload.html); 
    }

    if(evt.payload.exec){
      eval(evt.payload.exec); 
    }

    FnordMetric.resizeView();
  }

  function interval_modal_done(modal){
    var _sdate = $('.start_date', modal).val().match('([0-9]{2})\.([0-9]{2})\.([0-9]{4})')
    var _stime = $('.start_time', modal).val().match('([0-9]{2}):([0-9]{2})')
    var _edate = $('.end_date', modal).val().match('([0-9]{2})\.([0-9]{2})\.([0-9]{4})')
    var _etime = $('.end_time', modal).val().match('([0-9]{2}):([0-9]{2})')

    if (_sdate && _stime && _edate && _etime){
      var sd = new Date();
      var ed = new Date();
      sd.setHours(parseInt(_stime[1]), parseInt(_stime[2]), 0);
      sd.setFullYear(parseInt(_sdate[3]), parseInt(_sdate[2])-1, parseInt(_sdate[1]));
      ed.setHours(parseInt(_etime[1]), parseInt(_etime[2]), 0);
      ed.setFullYear(parseInt(_edate[3]), parseInt(_edate[2])-1, parseInt(_edate[1]));

      if (sd && ed && (ed.getTime() > sd.getTime())) {
        start_timestamp = parseInt(sd.getTime() / 1000);
        end_timestamp   = parseInt(ed.getTime() / 1000);

        load_interval();
        FnordMetric.ui.close_modal('body');
      }

    }
  }

  function open_interval_modal(){
    var modal = $('<div>');

    var interval_list = $('<ul>');
    var now = parseInt((new Date()).getTime() / 1000);

    var enter_catcher = function(e){
      if (e.which == 13) interval_modal_done(modal);
    }

    var offset_opener = function(e){
      var now = parseInt((new Date()).getTime() / 1000);

      if($(this).attr('data-offset-off')){
        now -= eval($(this).attr('data-offset-off'));
      }

      var off = eval($(this).attr('data-offset'));

      if (off > ctick) {
        start_timestamp = atTick((now-(off)));
        end_timestamp   = atTick(now);
      } else {
        start_timestamp = now-off;
        end_timestamp   = now;
      }

      load_interval();
      FnordMetric.ui.close_modal('body');
    }

    modal.append(
      $('<div>')
        .css({
          background: '#f2f2f2',
          borderBottom: "1px solid #ddd",
          padding: '12px 20px 10px 20px'
        })
        .append('<a class="button dark" style="height:30px; float:right; margin:1px; line-height:30px;">OK</a>')
        .append('<input class="input ropen start_date" style="width:100px;" placeholder="DD.MM.YYYY" />')
        .append('<input class="input lopen start_time" style="width:50px;" placeholder="HH:MM" />')
        .append('<span style="margin:0 15px; color:#999;">&mdash;</span>')
        .append('<input class="input ropen end_date" style="width:100px;" placeholder="DD.MM.YYYY" />')
        .append('<input type="text" class="input lopen end_time" style="width:50px;" placeholder="HH:MM" />')
      ).append($('<div style="padding:15px 20px;" class="offset_links ui_fancylinks">'));

    if (tick < 300){
      $('.offset_links', modal)
        .append('<b>Just now</b>')
        .append('<a data-offset="5*60">Last 5 Minutes</a> ')
        .append('<a data-offset="10*60">Last 10 Minutes</a> ')
        .append('<a data-offset="15*60">Last 15 Minutes</a> ')
        .append('<a data-offset="30*60">Last 30 Minutes</a> ')
        .append('<a data-offset="45*60">Last 45 Minutes</a><br /><br />');
    }

    if (tick < 4000){
      $('.offset_links', modal)
      .append('<b>Hourly</b>')
      .append('<a data-offset="3600">Last Hour</a> ')
      .append('<a data-offset="3600*3">Last 3 Hours</a> ')
      .append('<a data-offset="3600*6">Last 6 Hours</a> ')
      .append('<a data-offset="3600*12">Last 12 Hours</a> ')
      .append('<a data-offset="3600*18">Last 18 Hours</a><br /><br />');
    }

    $('.offset_links', modal)
      .append('<b>Daily</b>')
      .append('<a data-offset="3600*24">Today</a>, ')
      .append('<a data-offset="3600*24" data-offset-off="3600*24">Yesterday</a> ')
      .append('<a data-offset="3600*3*24">Last 3 Days</a> ')
      .append('<a data-offset="3600*7*24">Last 7 Days</a> ')
      .append('<a data-offset="3600*7*24*14">Last 14 Days</a> ')
      .append('<a data-offset="3600*7*24*30">Last 30 Days</a> ');

    $('.offset_links a', modal)
      .click(offset_opener);

    $.mask.definitions['~']='[0-5]';
    $.mask.definitions['%']='[0-2]';
    $.mask.definitions['$']='[0-2]';
    $.mask.definitions['#']='[0-3]';

    $('.button', modal)
      .click(function(){ interval_modal_done(modal) });

    $('.start_date', modal)
      .mask("#9.$9.2099")
      .keypress(enter_catcher)
      .val(FnordMetric.util.dateFormat(start_timestamp).split(" ")[0]);

    $('.start_time', modal)
      .mask("%9:~9")
      .keypress(enter_catcher)
      .val(FnordMetric.util.dateFormat(start_timestamp).split(" ")[1]);

    $('.end_date', modal)
      .mask("#9.$9.2099")
      .keypress(enter_catcher)
      .val(FnordMetric.util.dateFormat(end_timestamp).split(" ")[0]);

    $('.end_time', modal)
      .mask("%9:~9")
      .keypress(enter_catcher)
      .val(FnordMetric.util.dateFormat(end_timestamp).split(" ")[1]);


    /*for(n=0; n < 10; n++){
      var interval = now;
      now -= 3600;
      interval += "-" + now;
      interval_list.append(
        $('<li>')
          .html($('<a href="#">')
          .html(interval)
          .attr('data', interval)
          .click(function(){
            select_interval($(this).attr('data'))
          }))
      );
    }*/

    FnordMetric.ui.modal({
      height: 280,
      max_width: 570,
      content: modal
    });
  }

  function select_interval(inter){
    start_timestamp = parseInt(inter.split("-")[0]);
    end_timestamp = parseInt(inter.split("-")[1]);
    updateDatepicker();
    load_interval();
    FnordMetric.ui.close_modal(this);
  }

  function load_interval(){
    $('.gauge_viewport', viewport)
      .html("");

    FnordMetric.publish({
      "gauge": gauge_name,
      "type": "render_request",
      "interval": start_timestamp + "-" + end_timestamp
    })
  }

  return {
    load: load,
    resize: resize,
    announce: announce,
    close: close
  };

});
