var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;

  function renderDashboard(_dash){
    loadView(FnordMetric.views.dashboardView(_dash));
  };

  function renderSessionView(){
    loadView(FnordMetric.views.sessionView());
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
    loadView(FnordMetric.views.sessionView());
  };

  return {
    renderDashboard: renderDashboard,
    renderSessionView: renderSessionView,
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
