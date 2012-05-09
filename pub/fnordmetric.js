var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;
  var gaugeLoadRunning = false;

  var socket;

  function renderDashboard(_dash){
    loadView(FnordMetric.views.dashboardView(_dash));
  }

  function renderGauge(_gauge, gauge_conf){
    gaugeLoadRunning = false;
    loadView(FnordMetric.views.gaugeView(_gauge, gauge_conf));
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
    currentView.resize(
      canvasElem.innerWidth(),
      canvasElem.innerHeight()
    );
  };

  function init(_namespace, _canvasElem, _sock_addr){
    canvasElem = _canvasElem;
    FnordMetric.currentNamespace = _namespace;

    socket = new WebSocket(_sock_addr);
    socket.onmessage = socketMessage;
    socket.onclose = socketClose;
    socket.onopen = socketOpen;

    renderOverviewView();
  };

  function publish(obj){
    socket.send(JSON.stringify(obj));
  }

  function socketMessage(raw){
    console.log("Message: " + raw.data);
    var evt = JSON.parse(raw.data);

    if((evt._class == "render_response") && gaugeLoadRunning){
      renderGauge(evt._channel, evt.payload);
    } else {
      if(currentView){ currentView.announce(evt); }
    }
  }

  function socketOpen(){
    console.log("connected...");
  }

  function socketClose(){
    console.log("socket closed"); 
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
