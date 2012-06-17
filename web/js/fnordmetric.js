var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;
  var currentNamespace = false;
  var gauges = {};
  var socket, conf;

  var navigatedViaHash = false;

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
    var prev_active = false;

    if($('#sidebar li.active').length > 0){
      prev_active = $('#sidebar li.active:first').attr('data-token');
    }

    $('#sidebar')
      .html('');

    /*if(!conf.hide_overview){
      renderSidebarGroup('Overview')
        .append($('<li class="overview">')
          .append('<span class="picto piechart">')
          .append($('<a href="#" class="title">').html('App Overview')));
    }*/

    if(!conf.hide_active_users){
      renderSidebarGroup('Overview')
        .append($('<li class="overview">')
          .append($('<a href="#" class="title">').html('Active Users'))
          .click(function(){ renderSessionView(); }));
    }

    for(gkey in gauges){
      if(!gauges[gkey].group){ gauges[gkey].group = 'Gauges'; }
      if(!gauges[gkey].title){ gauges[gkey].title = gkey; }

      var ul = $('#sidebar ul[data-group="' + gauges[gkey].group + '"]');
      if(ul.length == 0){ ul = renderSidebarGroup(gauges[gkey].group); }

      ul.append($('<li class="gauge">')
        .attr('data-token', gkey)
        .attr('data-view', gauges[gkey].view_type)
        .append('<i class="icon-folder-close">')
        .append($('<a href="#" class="title">').html(gauges[gkey].title)));
    }

    $('#sidebar li').click(sidebarClick);

    if(prev_active){
      $('#sidebar li[data-token="'+prev_active+'"]').addClass('active');
    }
  }

  function sidebarClick(){
    $('#sidebar li').removeClass('active');
    $(this).addClass('active');

    if($(this).attr('data-view') == "dashboard"){
      FnordMetric.renderDashboard($(this).attr('data-token'));
      window.location.hash = 'dashboard/' + $(this).attr('data-token');  
    } else if($(this).attr('data-view') == "gauge"){ 
      FnordMetric.renderGauge($(this).attr('data-token'));
      window.location.hash = 'gauge/' + $(this).attr('data-token');
    }

    $(this).addClass('active');

    return false;
  }

  function addGauge(msg){
    if(!gauges[msg.gauge_key]){
      gauges[msg.gauge_key] = {
        "view_type": msg.view,
        "title": msg.title,
        "tick": msg.tick,
        "group": msg.group
      };

      renderSidebar();

      if(!currentView && (window.location.hash.length < 2)){
        sidebarClick.apply($('#sidebar li:first'));
      } else if(!navigatedViaHash){
        navigateViaHash();
      }
    }
  }

  function renderSessionView(){
    window.location.hash = 'active_users';
    loadView(FnordMetric.views.sessionView());
  }

  function renderOverviewView(){
    loadView(FnordMetric.views.overviewView());
  }

  function loadView(_view){
    FnordMetric.ui.close_modal('body');
    if(currentView){ currentView.close(); }
    canvasElem.html('loading!');
    currentView = _view;
    currentView.load(canvasElem);
    resizeView();
  };

  function resizeView(){
    var viewport_width = window.innerWidth - 220;
    if(viewport_width < 780){ viewport_width=780; }
    $('#viewport').width(viewport_width);
    $('.navbar').width(viewport_width);
    FnordMetric.ui.resizable('.viewport_inner');
    if(currentView){
      currentView.resize(
        canvasElem.innerWidth(),
        canvasElem.innerHeight()
      );  
    }
    $('.resize_full_height').height(window.innerHeight-40);
    $('#viewport .viewport_inner').css('minHeight', window.innerHeight-2);
    $('.resize_min_full_height').css('minHeight', window.innerHeight-2);
    $(".resize_listener").trigger('fm_resize');
  };


  function init(_conf){
    conf = _conf;
    this.currentNamespace = _conf.token;

    if(conf.title){ $('title').html(conf.title); }

    canvasElem = $("<div class='viewport_inner'>");
    canvasElem.addClass('clearfix');

    var _wrap_elem = $("<div id='wrap'>")
        .append($("<div id='sidebar'>"))
        .append($("<div id='viewport'>").append(canvasElem));

    connect();

    $('#app').html(_wrap_elem);
    $(window).resize(resizeView);    
    resizeView();
  };

  function connect(){
    socket = new WebSocket("ws://" + document.location.host + '/stream');
    socket.onmessage = socketMessage;
    socket.onclose = socketClose;
    socket.onopen = socketOpen;
  }

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
    $('.flash_msg_over').fadeOut(function(){ $(this).remove(); }); 
  }

  function socketClose(){
    console.log("socket closed"); 

    if($('.flash_msg_over').length == 0){
      $(viewport)
        .append($("<div class='flash_msg_over'>")
          .append($("<div class='inner'>")
            .append('<h1>Oopsiedaisy, lost the connection...</h1>')
            .append('<h2>Reconnecting to server...</h2>')
            .append('<div class="loader_white">')));

      window.setTimeout(function(){
        $('.flash_msg_over').addClass('visible');  
      }, 20);
    }

    window.setTimeout(connect, 1000); 
  }

  function navigateViaHash(){
    if (window.location.hash){
      var elem = null;
      if (!!window.location.hash.match(/^#dashboard\/[a-zA-Z_0-9-]+$/)) {
        elem = $('#sidebar li.gauge[data-token="'+window.location.hash.slice(11)+'"]');
      } else if (!!window.location.hash.match(/^#gauge\/[a-zA-Z_0-9-]+$/)){
        elem = $('#sidebar li.gauge[data-token="'+window.location.hash.slice(7)+'"]');
      } else if(window.location.hash == "#active_users") {
        navigatedViaHash = true;
        renderSessionView();
      }

      if (elem && (elem.length > 0)) {
        navigatedViaHash = true;
        elem.click();
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
    util: {},
    gauges: gauges
  };

})();
