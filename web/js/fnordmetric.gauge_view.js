FnordMetric.views.gaugeView = (function(gauge_name){
  var widgets = [];
  var widget_objs = {};
  var viewport = null;
  var tabs = [];
  var conf = {};

  function load(_viewport){
    viewport = _viewport;
    viewport.html('');
    viewport.append('<div class="navbar"></div>');
    viewport.append($('<div class="gauge_viewport"></div>'));

    conf.title = gauge_name;

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
    var init_interval = (now-(FnordMetric.gauges[gauge_name].tick*24)) + "-" + now;

    load_interval(init_interval);
  }

  function close(){

  }

  function resize(){   

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

  function open_interval_modal(){
    var interval_list = $('<ul>');
    var now = parseInt((new Date()).getTime() / 1000);

    for(n=0; n < 10; n++){
      var interval = now;
      now -= 3600;
      interval += "-" + now;
      interval_list.append(
        $('<li>')
          .html($('<a href="#">')
          .html(interval)
          .attr('data', interval)
          .click(function(){ 
            load_interval($(this).attr('data'));
            FnordMetric.ui.close_modal(this);
          }))
      );
    }

    FnordMetric.ui.modal({
      height: 700,
      content: interval_list
    });
  }

  function load_interval(interval){
    $('.gauge_viewport', viewport)
      .html("");

    FnordMetric.publish({
      "gauge": gauge_name,
      "type": "render_request",
      "interval": interval
    })
  }

  return {
    load: load,
    resize: resize,
    announce: announce,
    close: close
  };

});
