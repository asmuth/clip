var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;

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

  function init(_namespace, _canvasElem){
    canvasElem = _canvasElem;
    FnordMetric.currentNamespace = _namespace;
    renderOverviewView();
  };

  return {
    renderDashboard: renderDashboard,
    renderGauge: renderGauge,
    renderSessionView: renderSessionView,
    renderOverviewView: renderOverviewView,
    resizeView: resizeView,
    init: init,
    p: '',
    currentNamespace: false,
    currentWidgetUID: 23,
    ui: {},
    views: {},
    widgets: {},
    util: {}
  };

})();
