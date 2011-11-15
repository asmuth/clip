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
      (doPoll())();
      sessionView.poll = window.setInterval(doPoll(), 15000);
    };

    function doPoll(){
      return (function(){
        $.ajax({
          url: '/'+currentNamespace+'/sessions',
          success: callbackPoll()
        });
      });
    }

    function callbackPoll(){
      return (function(_data, _status){
        sessionData = JSON.parse(_data).sessions;
        renderSidebar();
      });
    };

    function formatTimeSince(time){
      var now = new Date().getTime()/1000;
      var since = now - time;
      if(since < 60){
        return parseInt(since) + 's';
      } else if(since<3600){
        return parseInt(since/60) + 'm';
      } else if(since<(3600*24)){
        return parseInt(since/3600) + 'h';
      } else {
        return ">1d"
      }
    }

    function renderSidebar(){
      var listElem = $('<ul class="session_list"></ul>');
      for(var sessionIndex=0; sessionIndex < sessionData.length; sessionIndex++){
        var session_data = sessionData[sessionIndex]
        var session_name = session_data["_name"];
        var session_picture = 'X';
    
        if(!session_name){ 
          session_name = session_data["session_key"].substr(0,15)
        };

        if(session_data["_picture"]){ 
          session_picture = $('<img width="18" />');
          session_picture.attr('src', session_data["_picture"]);
        };

        var session_time = formatTimeSince(session_data["_updated_at"]);

        listElem.append(
          $('<li class="session"></li>').append(
            $('<div class="picture"></div>').html(session_picture)
          ).append(
            $('<span class="name"></span>').html(session_name)
          ).append(
            $('<span class="time"></span>').html(session_time)
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