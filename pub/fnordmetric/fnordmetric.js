var FnordMetric = (function(){
 
  var canvasElem = false;

  var currentNamespace = false;
  var currentView = false;

  function decPrint(val){
    return (val < 10 ? '0'+val : val);
  }

  function formatTimeOfDay(_time){
    var time = new Date();
    time.setTime(_time*1000);
    return decPrint(time.getHours()) + ':' +
           decPrint(time.getMinutes()) + ':' +
           decPrint(time.getSeconds());
  }
  
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

  var timelineWidget = function(opts){
    
    function render(){

      var labels = opts.labels;
      var series = opts.series;

      var elem_id = "fm_graph_"+parseInt(Math.random()*99999);
      var elem_inner = $('.inner', opts.elem);
      elem_inner.append($('<div id="'+elem_id+'"></div>'));

      var width = elem_inner.width();
      var height = 240;
      var canvas = Raphael(elem_id, width, height+30);
      var xtick = width / (labels.length-1);

      var label_mod = Math.ceil((labels.length/10));

      var max = false; // each series has an individual y scale...
      
      $(series).each(function(n,_series){

        //var path_string = "M0,"+height;
        var path_string = "";
        var _max = max;

        if(!_max){ _max = Math.max.apply(Math, _series.data)*1.1; }

        $(_series.data).each(function(i,v){    

          var p_x = (i*xtick);
          var p_y = (height-((v/_max)*height));
            
          path_string += ( ( i == 0 ? "M" : "L" ) + p_x + ',' + p_y );

          if(i%label_mod==0){
            canvas.text(p_x, height+10, labels[i]).attr({
              font: '10px Helvetica, Arial', 
              fill: "#777"
            });
          }

          canvas.circle(p_x, p_y, 4).attr({
            fill: _series.color,
            stroke: '#fff',
            "stroke-width": 1, 
          }).toBack();


          var htrgt = canvas.rect(p_x - 20, p_y - 20, 40, 40).attr({
            stroke: "none", 
            fill: "#fff", 
            opacity: 0
          }).toFront();

          (function(htrgt){

            var t_y = p_y + 9;
            var ttt = canvas.text(p_x, t_y+10, v).attr({
              font: '12px Helvetica, Arial',
              fill: "#fff",
              opacity: 0
            });
            
            var tttb = ttt.getBBox();
            var ttw = tttb.width+20;
            var tt = canvas.rect(p_x-(ttw/2), t_y, ttw, 22, 5).attr({
              stroke: "none",
              fill: "#000",
              opacity: 0
            }).toBack();

            

            $(htrgt[0]).hover(function(){
              tt.animate({ opacity: 0.8 }, 300);
              ttt.animate({ opacity: 0.8 }, 300);
            }, function(){
              tt.animate({ opacity: 0 }, 300);
              ttt.animate({ opacity: 0 }, 300);
            });

          })(htrgt);

        });

        canvas.path(path_string).attr({
          stroke: _series.color, 
          "stroke-width": 3, 
          "stroke-linejoin": 'round'
        }).toBack(); 

        path_string += "L"+width+","+height+" L0,"+height+" Z";

        canvas.path(path_string).attr({
          stroke: "none", 
          fill: _series.color, 
          opacity: 0.1
        }).toBack();

      });

      canvas.drawGrid(0, 0, width, height, 1, 6, "#ececec");

    }

    return {
      render: render  
    };

  };

  var sessionView = (function(){
    
    var listElem = $('<ul class="session_list"></ul>');
    var feedInnerElem = $('<ul class="feed_inner"></ul>');
    var typeListElem  = $('<ul class="event_type_list"></ul>');
    var filterElem = $('<div class="events_sidebar"></div>').html(
      $('<div class="headbar"></div>').html('Event Types')
    ).append(typeListElem);
    var feedElem = $('<div class="sessions_feed"></div>').html(
      $('<div class="headbar"></div>').html('Event Feed')
    ).append(feedInnerElem);
    var sideElem = $('<div class="sessions_sidebar"></div>').html(
      $('<div class="headbar"></div>').html('Active Users')
    ).append(listElem);

    var eventsPolledUntil = false;
    var eventsFilter = [];
    var sessionData = {};
    var pollRunning = true;

    function load(elem){
      eventsPolledUntil = parseInt(new Date().getTime()/10000);
      elem.html('')
        .append(filterElem)
        .append(feedElem)
        .append(sideElem);
      startPoll();
      loadEventTypes();
    };

    function resize(_width, _height){
      $('.sessions_feed').width(_width-452);
    };

    function startPoll(){
      (doSessionPoll())();
      (doEventsPoll())();
      sessionView.session_poll = window.setInterval(doSessionPoll(), 1000);
    };

    function stopPoll(){
      pollRunning = false;
      window.clearInterval(sessionView.session_poll);
    }

    function doSessionPoll(){
      return (function(){
        $.ajax({
          url: '/'+currentNamespace+'/sessions',
          success: callbackSessionPoll()
        });
      });
    };

    function loadEventHistory(event_type){
      feedInnerElem.html('');
      $.ajax({
        url: '/'+currentNamespace+'/events?type='+event_type,
        success: function(_data, _status){
          var data = JSON.parse(_data).events;
          console.log(data);
          for(var n=data.length; n >= 0; n--){
            if(data[n]){ renderEvent(data[n]); }
          }
        }
      });
    }

    function callbackSessionPoll(){
      return (function(_data, _status){
        $.each(JSON.parse(_data).sessions, function(i,v){
          updateSession(v);  
        });
        sortSessions();
      });
    };

    function loadEventTypes(){
      $.ajax({
        url: '/'+currentNamespace+'/event_types',
        success: function(_data){
          var data = JSON.parse(_data);
          $(data.types).each(function(i,v){
            if(v.slice(0,5)!='_set_'){ addEventType(v,v); }
          });
        }
      });
    };

    function addEventType(type, display){
      typeListElem.append(
        $('<li class="event_type"></li>').append(
          $('<span class="history"></span>').html('history')
          .click(function(){ 
            $('.event_type_list .event_type input').attr('checked', false);
            $('input', $(this).parent()).attr('checked', true);
            updateEventFilter(); loadEventHistory(type); 
          })
        ).append(
          $('<input type="checkbox" />').attr('checked', true)
          .click(function(){ updateEventFilter(); })
        ).append(
          $('<span></span>').html(display)
        ).attr('rel', type)
      );
    }

    function updateEventFilter(){
      var _unchecked_types = [];
      $('ul.event_type_list li.event_type').each(function(i,v){
        if(!$('input', v).attr('checked')){
          _unchecked_types.push($(v).attr('rel'));
        }
      });
      eventsFilter = _unchecked_types;
    }

    function doEventsPoll(){
      return (function(){         
        $.ajax({
          url: '/'+currentNamespace+'/events?since='+eventsPolledUntil,
          success: callbackEventsPoll()
        });
      });
    };

    function callbackEventsPoll(){
      return (function(_data, _status){
        var data = JSON.parse(_data)
        var events = data.events;
        var timout = 1000;
        var maxevents = 200;
        if(events.length > 0){ 
          timeout = 200; 
          eventsPolledUntil = parseInt(events[0]._time)-1;
        }
	      for(var n=events.length-1; n >= 0; n--){
	        var v = events[n];
          if(eventsFilter.indexOf(v._type) == -1){
            if(parseInt(v._time)<=eventsPolledUntil){
              renderEvent(v);  
            }
          }
        };
        var elems = $("p", feedInnerElem);
        for(var n=maxevents; n < elems.length; n++){
          $(elems[n]).remove();
        }
        if(pollRunning){
          window.setTimeout(doEventsPoll(), timout);
        }
      });
    };

    function updateSession(session_data){
      sessionData[session_data.session_key] = session_data;
      renderSession(session_data);
    }

    function sortSessions(){
      console.log("fixme: sort and splice to 100");
    }

    function renderSession(session_data){

      var session_name = session_data["_name"];
      var session_time = formatTimeSince(session_data["_updated_at"]);
      var session_elem = $('li[data-session='+session_data["session_key"]+']:first');

      if(session_elem.length>0){

        if(session_data["_picture"] && (session_data["_picture"].length > 1)){
          $('.picture img', session_elem).attr('src', session_data["_picture"])
        }

        if(session_name){
          $('.name', session_elem).html(session_name);
        }
        
        $('.time', session_elem).html(session_time);

      } else {
        
        var session_picture = $('<img width="18" />');

        if(!session_name){ 
          session_name = session_data["session_key"].substr(0,15)
        };

        if(session_data["_picture"]){ 
          session_picture.attr('src', session_data["_picture"]);
        };
        
        listElem.append(
          $('<li class="session"></li>').append(
            $('<div class="picture"></div>').html(session_picture)
          ).append(
            $('<span class="name"></span>').html(session_name)
          ).append(
            $('<span class="time"></span>').html(session_time)
          ).attr('data-session', session_data["session_key"])
        );

      }
    };

    function renderEvent(event_data){
      var event_time = $('<span class="time"></span>');
      var event_message = $('<span class="message"></span>');
      var event_props = $('<span class="properties"></span>');
      var event_picture = $('<div class="picture"></picture>');

      var event_type = event_data._type;
      
      if(!event_type){ return true; }

      if(event_data._message){
        event_message.html(event_data._message);
      } else if(event_type=="_pageview"){
        event_message.html("Pageview: " + event_data.url);
      } else if(event_type.substr(0,5) == '_set_'){
        return true; /* dont render */
      } else {
        event_message.html(event_type);
      }

      event_time.html(formatTimeOfDay(event_data._time));
      
      if(event_data._session_key && event_data._session_key.length > 0){
        if(session_data=sessionData[event_data._session_key]){
          if(session_data._name){            
            event_props.append(
              $('<strong></strong>').html(session_data._name)
            );
          }
          if(session_data._picture){ 
            event_picture.append(
              $('<img width="40" />').attr('src', session_data._picture)
            )
          }
        }
      }

      feedInnerElem.prepend(
        $('<li class="feed_event"></li>')
        .append(event_time)
        .append(event_picture)
        .append(event_message)
        .append(event_props)
      );
    }

    function close(){
      stopPoll();
    };

    return {
      load: load,
      resize: resize,
      close: close
    };

  });
  

  var dashboardView = (function(dashboard_name){

    var widgets = [];
    var viewport = null;

    function load(_viewport){
      viewport = _viewport.html('');
      /*alert('yay, new dashboard view loaded: ' + dashboard_name);*/
      $.ajax({
        url: '/'+currentNamespace+'/dashboard/'+dashboard_name,
        success: function(resp, status){
          var conf = JSON.parse(resp);
          renderWidgets(conf.widgets);
        }
      });
    };

    function renderWidgets(_widgets){
      for(wkey in _widgets){
        var widget = _widgets[wkey];
        widget["elem"] = $('<div class="widget"></div>').append(
          $('<div class="headbar"></div>').html(widget.title)
        ).append(
          $('<div class="inner"></div>')
        );
        console.log(widget);
        widgets[wkey] = widget;
        viewport.append(widget.elem);
        resizeWidget(wkey);
        renderWidget(wkey);
      };
      resize();
    };

    function renderWidget(wkey){
      var widget = widgets[wkey];
      timelineWidget(widget).render();
    };

    function resizeWidget(wkey){
      var widget = widgets[wkey];
      var wwperc = widgets[wkey].width;
      if(!wwperc){ wwperc = 100; }
      var wwidth = viewport.width() * (wwperc/100.0);
      if(wwperc==100){
        widgets[wkey].elem.addClass('full_width');
      } else { wwidth -= 1; }
      widget.elem.width(wwidth);  
    }

    function resize(){
      for(wkey in widgets){
        resizeWidget(wkey);  
      };
    };

    function close(){
      
    };

    return {
      load: load,
      resize: resize,
      close: close
    };

  });


  function renderDashboard(_dash){  
    loadView(dashboardView(_dash));
  };

  function renderSessionView(){
    loadView(sessionView());
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
    currentNamespace = _namespace;
    loadView(sessionView());
  };

  return {
    p: '/fnordmetric/',  
    renderDashboard: renderDashboard,
    renderSessionView: renderSessionView,
    resizeView: resizeView,
    init: init
  };

})();
