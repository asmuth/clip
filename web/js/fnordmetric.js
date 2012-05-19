var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;
  var currentNamespace = false;
  var gauges = {};
  var socket, conf;

  function renderDashboard(_dash){
    loadView(FnordMetric.views.dashboardView(_dash));
  }

  function renderGauge(_gauge){
    loadView(FnordMetric.views.gaugeView(_gauge));
  }

  function renderSidebarGroup(grname){
    var ul = $("<ul>")
      .attr('data-group', grname);

    $('#sidebar')
      .append('<div class="ul_head">' + grname + '</div>')
      .append(ul);

    return ul;
  }
  
  function renderSidebar(){
    $('#sidebar')
      .html('');

    if(!conf.hide_overview){
      renderSidebarGroup('Overview')
        .append($('<li class="overview">')
          .append('<span class="picto piechart">')
          .append($('<a href="#" class="title">').html('App Overview')));
    }

    for(gkey in gauges){
      if(!gauges[gkey].group){ gauges[gkey].group = 'Gauges'; }
      if(!gauges[gkey].title){ gauges[gkey].title = gkey; }

      var ul = $('#sidebar ul[data-group="' + gauges[gkey].group + '"]');
      if(ul.length == 0){ ul = renderSidebarGroup(gauges[gkey].group); }

      ul.append($('<li class="gauge">')
        .attr('data-token', gkey)
        .attr('data-view', gauges[gkey].view_type)
        .append('<span class="picto piechart">')
        .append($('<a href="#" class="title">').html(gauges[gkey].title)));
    }

    $('#sidebar li').click(function(){
      $(this).addClass('active').siblings().removeClass('active');

      if($(this).attr('data-view') == "dashboard"){
        FnordMetric.renderDashboard($(this).attr('data-token'));
        window.location.hash = $(this).attr('data-token');  
      } else if($(this).attr('data-view') == "gauge"){ 
        FnordMetric.renderGauge($(this).attr('data-token'));
        window.location.hash = 'gauge/' + $(this).attr('data-token');
      }

      return false;
    });

  }

  function addGauge(msg){
    if(!gauges[msg.gauge_key]){
      gauges[msg.gauge_key] = {
        "view_type": msg.view,
        "title": msg.title,
        "group": msg.group
      };
      renderSidebar();
    }
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
    FnordMetric.ui.resizable('.viewport_inner');
    currentView.resize(
      canvasElem.innerWidth(),
      canvasElem.innerHeight()
    );
  };


  function init(_conf){
    conf = _conf;
    this.currentNamespace = _conf.token;

    if(conf.title){ $('title').html(conf.title); }

    canvasElem = $("<div class='viewport_inner'>");
    canvasElem.addClass('clearfix');

    socket = new WebSocket("ws://localhost:4243");
    socket.onmessage = socketMessage;
    socket.onclose = socketClose;
    socket.onopen = socketOpen;

    var _wrap_elem = $("<div id='wrap'>")
        .append($("<div id='sidebar'>"))
        .append($("<div id='viewport'>").append(canvasElem));

    $('#app').html(_wrap_elem);

    $(window).resize(resizeView);
    window.setTimeout(navigateViaHash, 200);
    
    resizeView();
  };

  function publish(obj){
    if(!obj.namespace){ 
      obj.namespace = FnordMetric.currentNamespace; 
    }
    socket.send(JSON.stringify(obj));
  }

  function socketMessage(raw){
    var evt = JSON.parse(raw.data);

    if((evt.type == "discover_response")){
      addGauge(evt);
    } else {
      if(currentView){ currentView.announce(evt); }
    }
  }

  function socketOpen(){
    console.log("connected...");
    publish({"type": "discover_request"});
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
    renderGauge: renderGauge,
    renderSessionView: renderSessionView,
    renderOverviewView: renderOverviewView,
    resizeView: resizeView,
    init: init,
    publish: publish,
    p: '',
    socket: socket,
    currentNamespace: null,
    currentWidgetUID: 23,
    ui: {},
    views: {},
    widgets: {},
    util: {}
  };

})();
