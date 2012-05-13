var fnordquery = (function(){

  var canvasElem = false;
  var currentView = false;

  // var socket;

  // function renderDashboard(_dash){
  //   loadView(FnordMetric.views.dashboardView(_dash));
  // }

  function load(_view){
    if(currentView){ currentView.close(); }
    canvasElem.html('loading!');
    currentView = _view;
    currentView.load(canvasElem);
    resizeView();
  };

  function resizeView(){
    var viewport_width = window.innerWidth - 220
    $('#viewport').width(viewport_width);
    // currentView.resize(
    //   canvasElem.innerWidth(),
    //   canvasElem.innerHeight()
    // );
  };


  function init(){
    canvasElem = $("<div class='viewport_inner'>");
    canvasElem.addClass('clearfix');

    // socket = new WebSocket(_sock_addr);
    // socket.onmessage = socketMessage;
    // socket.onclose = socketClose;
    // socket.onopen = socketOpen;

    var wrap_elem = $("<div id='wrap'>")
        .append($("<div id='sidebar'>").append('<ul>'))
        .append($("<div id='viewport'>").append(canvasElem));

    $("body").html(wrap_elem);

    $(window).resize(resizeView);
    window.setTimeout(navigateViaHash, 200);
    
    resizeView();
  };

  // function publish(obj){
  //   socket.send(JSON.stringify(obj));
  // }

  // function socketMessage(raw){
  //   var evt = JSON.parse(raw.data);

  //   if((evt._class == "render_response") && gaugeLoadRunning){
  //     renderGauge(evt._channel, evt.payload);
  //   } else if((evt._class == "discover_response")){
  //     addGauge(evt);
  //   } else {
  //     if(currentView){ currentView.announce(evt); }
  //   }
  // }

  // function socketOpen(){
  //   console.log("connected...");
  //   publish({"_class": "discover_request"});
  // }

  // function socketClose(){
  //   console.log("socket closed"); 
  // }

  function navigateViaHash(){
    if(window.location.hash){
      // if(!!window.location.hash.match(/^#dashboard\/[a-zA-Z_0-9-]+$/)) {
      //   $('#sidebar li.dashboard[rel="'+window.location.hash.slice(11)+'"]').trigger('click');
      // } else if (!!window.location.hash.match(/^#gauge\/[a-zA-Z_0-9-]+$/)){
      //   $('#sidebar li.gauge[rel="'+window.location.hash.slice(7)+'"]').click();
      // }
    }
  }

  return {
    init: init,
    load: load,
    widgets: {},
    ui: {},
    views: {}
  };

})();
