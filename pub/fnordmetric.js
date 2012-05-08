var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;
  var gaugeLoadRunning = false;

  var socket;

  function renderDashboard(_dash){
    loadView(FnordMetric.views.dashboardView(_dash));
  }

  function renderGauge(_gauge){
    gaugeLoadRunning = false;
    console.log('render!!!')
    loadView(FnordMetric.views.gaugeView(_gauge));
  }

  function renderGaugeAsync(_gauge){
    gaugeLoadRunning = true;
    socket.send(JSON.stringify({
      "_channel": _gauge,
      "_class": "render_request"
    }))
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

  function socketMessage(raw){
    console.log("Message: " + raw.data);
    var evt = JSON.parse(raw.data);

    if((evt._class == "render_response") && gaugeLoadRunning){
      renderGauge(evt._channel);
    }
  }

  function socketOpen(){
    console.log("connected...");
    socket.send(JSON.stringify({ "fnord": "bar" }));
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
