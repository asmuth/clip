var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;

  var socket;

  function renderDashboard(_dash){
    loadView(FnordMetric.views.dashboardView(_dash));
  }

  function renderGauge(_gauge){
    loadView(FnordMetric.views.gaugeView(_gauge));
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

  function socketMessage(event){
    console.log("Message: " + event.data);
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
    renderGauge: renderGauge,
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
