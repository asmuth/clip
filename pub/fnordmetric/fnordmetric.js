var FnordMetric = (function(){
 
  var canvasElem = false;

  var currentNamespace = false;
  var currentView = false;

  var sessionView = (function(){
    
    var feedElem = $('<div class="sessions_feed"></div>');
    var sideElem = $('<div class="sessions_sidebar"></div>');
    var sessionData;

    function load(elem){
      elem.html('').append(feedElem).append(sideElem);
      startPoll();
    };

    function resize(_width, _height){
      $('.sessions_feed').width(_width-251);
    };

    function startPoll(){
      sessionView.poll = window.setInterval(function(){
        $.ajax({
          url: '/'+currentNamespace+'/sessions',
          success: callbackPoll()
        });
      }, 500);
    };

    function callbackPoll(){
      return (function(_data, _status){
        sessionData = JSON.parse(_data).sessions;
        renderSidebar();
      });
    };

    function renderSidebar(){
      var listElem = $('<ul class="session_list"></ul>');
      for(session_key in sessionData){
        listElem.append(
          $('<li class="session"></li>').append(
            $('<div class="picture"></div>')
          ).append(
            $('<span class="name"></span>').html(session_key)
          ).append(
            $('<span class="time"></span>').html('23min')
          )
        );
      }
      sideElem.html(
        $('<div class="headbar"></div>').html('Active Users')
      );
      sideElem.append(listElem);
    };

    function close(){
      
    };

    return {
      load: load,
      resize: resize,
      close: close
    };

  })();
  
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
  }

  function init(_namespace, _canvasElem){
    canvasElem = _canvasElem;
    currentNamespace = _namespace;
    loadView(sessionView);
  };

  return {
    p: '/fnordmetric/',  
    loadView: loadView,
    resizeView: resizeView,
    init: init
  };

})();