FnordMetric.views.sessionView = (function(){

  var listElem = $('<ul class="session_list"></ul>');
  var feedInnerElem = $('<ul class="feed_inner"></ul>');
  var typeListElem  = $('<ul class="event_type_list"></ul>');

  var filterElem = $('<div class="events_sidebar"></div>')
    .append("<div class='headbar small'>Event Types</div>")
    .append(typeListElem);

  var feedElem = $('<div class="sessions_feed"></div>').append(feedInnerElem);
  var sideElem = $('<div class="sessions_sidebar"></div>').append(listElem);

  var eventsPolledUntil = false;
  var eventsFilter = {uncheckedTypes: [], checkedSessions: []};
  var sessionData = {};
  var pollRunning = true;
  var first_poll  = true;

  function load(elem){
    eventsPolledUntil = parseInt(new Date().getTime()/10000);

    elem.html('')
      .append('<div class="navbar"></div>');

    FnordMetric.ui.navbar($('.navbar', viewport), {
      breadcrumb: [
        ["Fnord", "/group/fnord"],
        ["Active Users", "/active_users"]
      ],
      buttons: [
        ["<i class='icon-refresh'></i>Refresh", function(){  }]
      ]
    });

    elem
      .append(filterElem.css('marginTop', '47px'))
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
    FnordMetric.views.sessionView.session_poll = window.setInterval(doSessionPoll(), 1000);
  };

  function stopPoll(){
    pollRunning = false;
    window.clearInterval(FnordMetric.views.sessionView.session_poll);
  }

  function doSessionPoll(){
    return (function(){
      /*$.ajax({
        url: FnordMetric.p + '/' + FnordMetric.currentNamespace+'/sessions',
        success: callbackSessionPoll()
      });*/
      sortSessions();
    });
  };

  function loadEventHistory(params){
    feedInnerElem.html('');
    /*$.ajax({
      url: FnordMetric.p + '/' + FnordMetric.currentNamespace+'/events',
      data: params,
      success: function(_data, _status){
        var data = JSON.parse(_data).events;
        for(var n=data.length; n >= 0; n--){
          if(data[n]){ renderEvent(data[n]); }
        }
      }
    });*/
  }

  function callbackSessionPoll(){
    return (function(_data, _status){
      $.each(JSON.parse(_data).sessions, function(i,v){
        updateSession(v);
      });
      sortSessions();
    });
  };

  function loadEventTypes(data){
    typeListElem.html('');
    $(data).each(function(i,v){
      if((v.length > 0) && (v.slice(0,5)!='_set_')){
        addEventType(v,v);
      }
    });
  };

  function setCheckboxesCheckedState(types_state, sessions_state) {
    $('.event_type_list .event_type input').attr('checked', types_state);
    $('.session_list .session input').attr('checked', sessions_state);
  }

  function addEventType(type, display){
    typeListElem.append(
      $('<li class="event_type"></li>').append(
        $('<span class="history"></span>').html('history')
        .click(function(){
          setCheckboxesCheckedState(true, true);
          $('input', $(this).parent()).attr('checked', true);
          updateEventFilter(); loadEventHistory({type: type});
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
    eventsFilter.uncheckedTypes = _unchecked_types;

    var _checked_sessions = [];
    $('ul.session_list li.session').each(function(i,v){
      if($('input', v).attr('checked')){
        _checked_sessions.push($(v).data().session);
      }
    });
    eventsFilter.checkedSessions = _checked_sessions;
  }

  function doEventsPoll(){
    return (function(){

      /*$.ajax({
        url: FnordMetric.p + '/' + FnordMetric.currentNamespace+'/events?since='+eventsPolledUntil,
        success: callbackEventsPoll()
      });*/

      FnordMetric.publish({
        "type": "active_users_request",
        "first_poll": first_poll,
        "since": eventsPolledUntil
      });

      first_poll = false;

    });
  };

  function announce(evt){
    if (evt.type == "active_users_response"){
      callbackEventsPoll(evt);
      $(evt.sessions).each(function(){ updateSession(this); });
      if(evt.types.length > 0){ loadEventTypes(evt.types); }
    }
  }

  function callbackEventsPoll(data){
    var events = data.events;
    var timeout = 1000;
    var maxevents = 200;
    var passesFiltering = function(event_data) {
      var passes_type_filtering = false;
      var passes_session_filtering = false;
      if(eventsFilter.uncheckedTypes.indexOf(event_data._type) == -1) {
        if(parseInt(v._time)<=eventsPolledUntil) {
          passes_type_filtering = true;
        }
      }
      if(!passes_type_filtering) return false;

      if(eventsFilter.checkedSessions.length == 0){
        return true; // No filter set - show all events
      } else {
        if(event_data._session_key){
          if(eventsFilter.checkedSessions.indexOf(event_data._session_key) >= 0){
            return true; // Filter set and match
          } else {
            return false; // Filter set but no match
          }
        } else {
          return false; // Filter set but event is not associated with session
        }
      }
    }

    if(events.length > 0){
      eventsPolledUntil = parseInt(events[0]._time)-1;
    }
    for(var n=events.length-1; n >= 0; n--){
      var v = events[n];
      if(passesFiltering(v)) {
        renderEvent(v);
      }
    };
    var elems = $("p", feedInnerElem);
    for(var n=maxevents; n < elems.length; n++){
      $(elems[n]).remove();
    }
    if(pollRunning){
      window.setTimeout(doEventsPoll(), timeout);
    }
  };

  function updateSession(session_data){
    var session_key = session_data.session_key;
    if(!sessionData[session_key]){
      updateEventFilter()
    }
    sessionData[session_key] = session_data;
    renderSession(session_data);
  }

  function sortSessions(){
    console.log("fixpaul: sort and splice to 100");
  }

  function renderSession(session_data){

    var session_name = session_data["_name"];
    var session_time = FnordMetric.util.formatTimeSince(session_data["_updated_at"]);
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

      var session_picture = $('<img width="25" />');

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
        ).append(
          $('<span class="history"></span>').html('history')
          .click(function(){
            setCheckboxesCheckedState(true, false);              
            updateEventFilter(); 
            loadEventHistory({session_key: session_data["session_key"]});
          })
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

    event_time.html(FnordMetric.util.formatTimeOfDay(event_data._time));

    if(event_data._session_key && event_data._session_key.length > 0){
      var __session_key = event_data._session_key;
      var load_usersession = (function(){
        loadEventHistory({session_key: __session_key});
      });
      if(session_data=sessionData[event_data._session_key]){
        if(session_data._name){
          event_props.append(
            $('<strong></strong>').html(session_data._name).css({
              'cursor': 'pointer'
            }).click(load_usersession)
          );
        }
        if(session_data._picture){
          event_picture.append(
            $('<img width="40" />').attr('src', session_data._picture)
          ).click(load_usersession);
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
    announce: announce,
    close: close
  };

});
