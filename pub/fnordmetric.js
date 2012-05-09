var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;
  var gaugeLoadRunning = false;
  var gauges = {};

  var socket;

  function renderDashboard(_dash){
    loadView(FnordMetric.views.dashboardView(_dash));
  }

  function renderGauge(_gauge, gauge_conf){
    gaugeLoadRunning = false;
    loadView(FnordMetric.views.gaugeView(_gauge, gauge_conf));
  }

  function renderSidebar(){
    $('#sidebar ul').html();

    for(gkey in gauges){
      $('#sidebar ul').append($('<li class="gauge">')
        .attr('rel', gkey)
        .append('<span class="picto piechart">')
        .append($('<span class="title">').html(gauges[gkey].title))
        .append('<span class="meta">'));
    }

    $('#sidebar li.dashboard').click(function(){
      FnordMetric.renderDashboard($(this).attr('rel'));
      window.location.hash = $(this).attr('rel');
    });

    $('#sidebar li.gauge').click(function(){
      FnordMetric.renderGauge($(this).attr('rel'));
      window.location.hash = 'gauge/' + $(this).attr('rel');
    });

    $('#sidebar li').click(function(){
      $(this).addClass('active').siblings().removeClass('active');
    });

  }

  function addGauge(msg){
    if(!gauges[msg.gauge_key]){
      gauges[msg.gauge_key] = {};
      renderSidebar();
    }
  }

  function renderGaugeAsync(_gauge){
    gaugeLoadRunning = true;
    publish({
      "_channel": _gauge,
      "_class": "render_request"
    })
  }

  function renderSessionView(){
    loadView(FnordMetric.views.sessionView());
  }

  function renderOverviewView(){
    loadView(FnordMetric.views.overviewView());
  }

  function loadView(_view){
    if(currentView){ currentView.close(); }
    canvasElem.html('loading!');
    currentView = _view;
    currentView.load(canvasElem);
    resizeView();
  };

  function resizeView(){
    var viewport_width = window.innerWidth - 220
    $('#viewport').width(viewport_width);
    currentView.resize(
      canvasElem.innerWidth(),
      canvasElem.innerHeight()
    );
  };


  function init(_canvasElem, _sock_addr){
    canvasElem = $("<div class='viewport_inner'>");
    canvasElem.addClass('clearfix');

    socket = new WebSocket(_sock_addr);
    socket.onmessage = socketMessage;
    socket.onclose = socketClose;
    socket.onopen = socketOpen;

    var _wrap_elem = $("<div id='wrap'>")
        .append($("<div id='sidebar'>").append('<ul>'))
        .append($("<div id='viewport'>").append(canvasElem));

    _canvasElem.html(_wrap_elem);

    $(window).resize(resizeView);
    window.setTimeout(navigateViaHash, 200);
    
    resizeView();
  };

  function publish(obj){
    socket.send(JSON.stringify(obj));
  }

  function socketMessage(raw){
    console.log("Message: " + raw.data);
    var evt = JSON.parse(raw.data);


    if((evt._class == "render_response") && gaugeLoadRunning){
      renderGauge(evt._channel, evt.payload);
    } else if((evt._class == "discover_response")){
      addGauge(evt);
    } else {
      if(currentView){ currentView.announce(evt); }
    }
  }

  function socketOpen(){
    console.log("connected...");
    publish({"_class": "discover_request"});
  }

  function socketClose(){
    console.log("socket closed"); 
  }

  function navigateViaHash(){
    if(window.location.hash){
      if(!!window.location.hash.match(/^#dashboard\/[a-zA-Z_0-9-]+$/)) {
        $('#sidebar li.dashboard[rel="'+window.location.hash.slice(11)+'"]').trigger('click');
      } else if (!!window.location.hash.match(/^#gauge\/[a-zA-Z_0-9-]+$/)){
        $('#sidebar li.gauge[rel="'+window.location.hash.slice(7)+'"]').click();
      }
    }
  }

  return {
    renderDashboard: renderDashboard,
    renderGauge: renderGaugeAsync,
    renderSessionView: renderSessionView,
    renderOverviewView: renderOverviewView,
    resizeView: resizeView,
    init: init,
    publish: publish,
    p: '',
    socket: socket,
    currentNamespace: false,
    currentWidgetUID: 23,
    ui: {},
    views: {},
    widgets: {},
    util: {}
  };

})();
