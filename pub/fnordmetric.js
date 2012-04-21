var FnordMetric = (function(){

  var canvasElem = false;

  var currentNamespace = false;
  var currentView = false;
  var currentWidgetUID=23;

  function decPrint(val){
    return (val < 10 ? '0'+val : val);
  }

  function formatTimeOfDay(_time){
    if(_time === null) {
      return "";
    } else {
      var time = new Date();
      time.setTime(_time*1000);
      return decPrint(time.getHours()) + ':' +
             decPrint(time.getMinutes()) + ':' +
             decPrint(time.getSeconds());
    }
  }

  function formatTimeRange(range){
    if (range < 60){
      return parseInt(range) + ' sec';
    } else if(range<3600){
      return parseInt(range/60) + ' min';
    } else if(range==3600){
      return '1 hour';
    } else if(range<(3600*24)){
      return parseInt(range/3600) + ' hours';
    } else if(range==(3600*24)){
      return '1 day';
    } else {
      return parseInt(range/(3600*24)) + ' days';
    }
  }

  function formatTimeSince(time){
    var now = new Date().getTime()/1000;
    var since = now - time;
    return formatTimeRange(since);
  }

  function formatOffset(offset, next_offset){
    if((offset == 0) && (next_offset==(3600*24))){
      return 'today';
    } if((offset == 0) && (next_offset==3600)){
      return 'this hour';
    } else if(offset == 0){
      return 'last ' + formatTimeRange(next_offset||0);
    } else if(offset==(3600*24)){
      return 'yesterday';
    } else if(offset==3600){
      return 'last hour';
    } else {
      return formatTimeRange(offset) + ' ago';
    }
  }

  function formatValue(value){
    if(value < 10){
      return value.toFixed(2);
    } else if(value > 1000){
      return (value/1000.0).toFixed(1) + "k";
    } else {
      return value.toFixed(0);
    }
  }

  function formatTimeValue(value){
    if (value < 60){
      return parseFloat(value).toFixed(1) + 's';
    } else if(value<3600){
      return parseFloat(value/60).toFixed(1) + 'm';
    } else if(value<(3600*24)){
      return parseFloat(value/3600).toFixed(1) + 'h';
    } else {
      return parseFloat(value/(3600*24)).toFixed(1) + 'd';
    }
  }

  function formatPercentValue(value){
    return value + '%';  
  }

  function formatGaugeValue(gauge_key, value){
    if(gauge_key.slice(0,8) === '__time__'){
      return formatTimeValue(value);
    } else if(gauge_key.slice(0,11) === '__percent__'){
      return formatPercentValue(value);
    } else {
      return formatValue(value);
    }
  }

  function getNextWidgetUID(){
    return (currentWidgetUID += 1);
  }

 var toplistWidget = function(){

    function render(opts){

      var current_gauge = false;

      var headbar = $('<div class="headbar"></div>').append(
        $('<h2></h2>').html(opts.title)
      );

      opts.elem.append(headbar).css({
        'marginBottom': 20,
        'overflow': 'hidden'
      }).append(
        $('<div class="toplist_inner"></div>')
      );

      var first = true;
      for(k in opts.gauges){
        headbar.append(
          $('<div></div>')
            .attr('class', 'button mr')
            .attr('rel', k)
            .append(
              $('<span></span>').html(opts.gauges[k].title)
            ).click(function(){
              loadGauge($(this).attr('rel'));
            }
          )
        );
        if(first){
          first = false;
          loadGauge(k);
        }
      }

      if(opts.autoupdate){
        var secs = parseInt(opts.autoupdate);
        if(secs > 0){

          var autoupdate_interval = window.setInterval(function(){
            loadGauge(false, true);
          }, secs*1000);

          $('body').bind('fm_dashboard_close', function(){
            window.clearInterval(autoupdate_interval);
          });
        }
      };

      function loadGauge(gkey, silent){
        if(!gkey){ gkey = current_gauge; }
        current_gauge = gkey;
        if(!silent){ $('.toplist_inner', opts.elem).addClass('loading'); }
        var _url = FnordMetric.p + '/' + currentNamespace + '/gauge/' + gkey;
        $.get(_url, function(_resp){
          var resp = JSON.parse(_resp);
          renderGauge(gkey, resp);
        })
      }

      function renderGauge(gkey, gdata){
        var _elem = $('.toplist_inner', opts.elem).removeClass('loading').html('');
        $(gdata.values).each(function(n, _gd){
          var _perc  = (parseInt(gdata.values[n][1]) / parseFloat(gdata.count))*100;
          var _item = $('<div class="toplist_item"><div class="title"></div><div class="value"></div><div class="percent"></div></div>');
          $('.title', _item).html(gdata.values[n][0]);
          $('.value', _item).html(formatGaugeValue(gkey, parseInt(gdata.values[n][1])));
          $('.percent', _item).html(_perc.toFixed(1) + '%');
          _elem.append(_item);
        });
      }

    }


    return {
      render: render
    };

  };

	var htmlWidget = function(){
		function render(opts){

      opts.elem.append(
        $('<div class="headbar small"></div>').html(opts.title)
      ).css({
        'marginBottom': 20,
        'overflow': 'hidden'
      });
			
			var container = $('<div></div>')
        .addClass('html_container')
				.html(opts.html);
			
			opts.elem.append(container);
		}
		
		function updateValues(opts){}
		function updatedisplay(opts, diff_factor){}
	
		return {
			render: render
		};
	}

  var numbersWidget = function(){


    function render(opts){

      opts.elem.append(
        $('<div class="headbar small"></div>').html(opts.title)
      ).css({
        'marginBottom': 20,
        'overflow': 'hidden'
      });

      for(k in opts.gauges){
        var gtick = parseInt(opts.gauges[k].tick);
        var gtitle = opts.gauges[k].title;

        var container = $('<div></div>')
          .addClass('numbers_container')
          .addClass('size_'+opts.offsets.length)
          .attr('rel', k)
          .append(
            $('<div></div>')
              .addClass('title')
              .html(gtitle)
          );


        $(opts.offsets).each(function(n, offset){
          var _off, _nextoff, _sum;
          if (offset[0]=="s"){
            _off = 0;
            _sum = _nextoff = (gtick * parseInt(offset.slice(1)));
          } else {
            _sum = 0;
            _off = offset*gtick;
            _nextoff = gtick;
          }
          container.append(
            $('<div></div>')
              .addClass('number')
              .attr('rel', k)
              .attr('data-offset', _off)
              .attr('data-sum', _sum)
              .attr('data',0)
              .append(
                $('<span></span>').addClass('desc').html(formatOffset(_off, _nextoff))
              )
              .append(
                $('<span></span>').addClass('value').html(0)
              )
            );
        });

        opts.elem.append(container);
      }

      if(opts.autoupdate){
        var secs = parseInt(opts.autoupdate);
        if(secs > 0){

          var autoupdate_interval = window.setInterval(function(){
            updateValues(opts);
          }, secs*1000);

          $('body').bind('fm_dashboard_close', function(){
            window.clearInterval(autoupdate_interval);
          });

        }

      };

      updateValues(opts);

    }

    function updateValues(opts){
      var values = $('.number', $(opts.elem));
      var values_pending = values.length;
      values.each(function(){
        var _sum = parseInt($(this).attr('data-sum'));
        var num = this;
        var at = parseInt(new Date().getTime()/1000);
        var url = FnordMetric.p + '/' + currentNamespace + '/gauge/' + $(this).attr('rel');
        if(_sum > 0){
          url += '?at='+(at-_sum)+'-'+at+'&sum=true';
        } else {
          at -= parseInt($(this).attr('data-offset'));
          url += '?at='+at;
        }

        $.get(url, function(_resp){
          var resp = JSON.parse(_resp);
          for(_k in resp){
            $(num).attr('data', (resp[_k]||0));
          }
          if((values_pending -= 1)==0){
            updateDisplay(opts, 4);
          }
        });
      });
    }

    function updateDisplay(opts, diff_factor){
      var still_running = false;
      $('.number', $(opts.elem)).each(function(){
        var target_val = parseFloat($(this).attr('data'));
        var current_val = parseFloat($(this).attr('data-current'));
        if(!current_val){ current_val=0; }
        var diff = (target_val-current_val)/diff_factor;
        if(diff < 1){ diff=1; }
        if(target_val > current_val){
          still_running = true;
          var new_val = current_val+diff;
          if(new_val > target_val){ new_val = target_val; }
          $(this).attr('data-current', new_val);
          $('.value', this).html(formatGaugeValue($(this).attr('rel'), new_val));
        }
      });
      if(still_running){
        (function(df){
          window.setTimeout(function(){ updateDisplay(opts, df); }, 30);
        })(diff_factor);
      }
    }

    return {
      render: render
    };

  };

  var timelineWidget = function(){

    function render(opts){

      var widget_uid = getNextWidgetUID();
      var chart=false;
      var max_y=0;

      function redrawWithRange(first_time, silent){
        if(!silent){ $(opts.elem).css('opacity', 0.5); }
        redrawDatepicker();
        var _query = '?at='+opts.start_timestamp+'-'+opts.end_timestamp;
        //chart.series = [];
        max_y=0;
        //metrics_completed = 0;
        $(opts.gauges).each(function(i,gauge){
          $.ajax({
            url: FnordMetric.p + '/' + currentNamespace +'/gauge/'+gauge+_query,
            success: redrawGauge(first_time, gauge)
          });
        });
      }

      function redrawGauge(first_time, gauge){
        return (function(json){
          var raw_data = JSON.parse(json);
          var series_data = [];

          for(p in raw_data){
            series_data.push([parseInt(p)*1000, raw_data[p]||0]);
            max_y = Math.max(max_y, raw_data[p]);
          }

          if(!first_time){
            chart.get('series-'+gauge).setData(series_data);
          } else {
            chart.addSeries({
              name: opts.gauge_titles[gauge],
              data: series_data,
              id: 'series-'+gauge
            });
          }

          chart.yAxis[0].setExtremes(0,max_y);
          chart.redraw();

          // shown on the *first* gauge load
          $(opts.elem).css('opacity', 1);
        });
      }

      function redrawDatepicker(){
        $('.datepicker', opts.elem).html(
          Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.start_timestamp)*1000) +
          '&nbsp;&dash;&nbsp;' +
          Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.end_timestamp)*1000)
        );
      }

      function moveRange(direction){
        v = opts.tick*direction*8;
        opts.start_timestamp += v;
        opts.end_timestamp += v;
        redrawWithRange();
      }

      function drawLayout(){
        $(opts.elem).append( $('<div></div>').attr('class', 'headbar').append(
          $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('refresh')).click(
            function(){ redrawWithRange(); }
          )
        ).append(
          $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('&rarr;')).click(
            function(){ moveRange(1); }
          )
        ).append(
          $('<div></div>').attr('class', 'datepicker')
        ).append(
          $('<div></div>').attr('class', 'button').append($('<span></span>').html('&larr;')).click(
            function(){ moveRange(-1); }
          )
        ).append(
          $('<h2></h2>').html(opts.title)
        ) ).append(
          $('<div></div>').attr('id', 'container-'+widget_uid).css({
            height: 256,
            marginBottom: 20,
            overflow: 'hidden'
          })
        );
      }

      function drawChart(){
        chart = new Highcharts.Chart({
          chart: {
            renderTo: 'container-'+widget_uid,
            defaultSeriesType: opts.plot_style,
            height: 270
          },
          series: [],
          title: { text: '' },
          xAxis: {
            type: 'datetime',
            tickInterval: opts.tick * 1000,
            title: (opts.x_title||''),
            labels: { step: 2 }
          },
          yAxis: {
            title: (opts.y_title||''),
            min: 0,
            max: 1000
          },
          legend: {
            layout: 'horizontal',
            align: 'top',
            verticalAlign: 'top',
            x: -5,
            y: -3,
            margin: 25,
            borderWidth: 0
          },
          plotOptions: {
            line: {
              shadow: false,
              lineWidth: 3
            }
          }
        });
      }

      drawLayout();
      drawChart();

      redrawWithRange(true);

      if(opts.autoupdate){
        var secs = parseInt(opts.autoupdate);
        if(secs > 0){

          var autoupdate_interval = window.setInterval(function(){
            if(
              (parseInt(new Date().getTime()/1000) - opts.end_timestamp) >
              (opts.include_current ? 0 : opts.tick)
            ){
              opts.end_timestamp += opts.tick;
              opts.start_timestamp += opts.tick;
            }

            redrawWithRange(false, true);
          }, secs*1000);

          $('body').bind('fm_dashboard_close', function(){
            window.clearInterval(autoupdate_interval);
          });

        }
      };

    }

    return {
      render: render
    };

  };


  var barsWidget = function(){

    function render(opts){

      var widget_uid = getNextWidgetUID();
      var chart=false;
      var max_y=0;

      function redraw(first_time, silent){
        if(!silent){ $(opts.elem).css('opacity', 0.5); }
        max_y=0;
        $(opts.gauges).each(function(i,gauge){
          $.ajax({
            url: FnordMetric.p + '/' + currentNamespace +'/gauge/'+gauge,
            success: redrawGauge(first_time, gauge)
          });
        });
      }

      function redrawGauge(first_time, gauge){
        return (function(json){
          var raw_data = JSON.parse(json);
          var series_data = [];
          var series_type;
          var label_data = [];

          if(opts.plot_style == 'horizontal'){
            series_type = 'bar';
          } else {
            series_type = 'column';
          }

          if(opts.order_by == 'field'){
            raw_data.values.sort(function(a,b){
              if(a[0] == b[0]){
                return 0;
              }else if(a[0] > b[0]){
                return 1;
              } else {
                return -1;
              }
            });
          }

          for(p in raw_data.values){
            label_data.push(raw_data.values[p][0]||'?');
            series_data.push(parseInt(raw_data.values[p][1]||0));
            //max_y = Math.max(max_y, raw_data[p]);
          }

          chart = new Highcharts.Chart({
            chart: {
              renderTo: 'container-'+widget_uid,
              defaultSeriesType: series_type,
              height: 270
            },
            title: { text: '' },
            xAxis: {
             categories: label_data
            },
            yAxis: {
              title: { text: '' }
            },
            legend: {
              layout: 'horizontal',
              align: 'top',
              verticalAlign: 'top',
              x: -5,
              y: -3,
              margin: 25,
              borderWidth: 0
            },
            plotOptions: {
              column: {
                animation: first_time,
              }
            },
            series: [
              {
                name: opts.gauge_titles[gauge],
                data: series_data
              }
            ]
          });

          //chart.redraw();
          $(opts.elem).css('opacity', 1);
        });
      }

      function drawLayout(){
        $(opts.elem).append( $('<div></div>').attr('class', 'headbar').append(
          $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('refresh')).click(
            function(){ redraw(); }
          )
        ).append(
          $('<h2></h2>').html(opts.title)
        ) ).append(
          $('<div></div>').attr('id', 'container-'+widget_uid).css({
            height: 256,
            marginBottom: 20,
            overflow: 'hidden'
          })
        );
      }

      drawLayout();
      redraw(true);

      if(opts.autoupdate){
        var autoupdate_interval = window.setInterval(function(){
          redraw(false, true);
        }, opts.autoupdate*1000);

        $('body').bind('fm_dashboard_close', function(){
          window.clearInterval(autoupdate_interval);
        });
      }

    }

    return {
      render: render
    };

  };

  var pieWidget = function(){

    function render(opts){

      var widget_uid = getNextWidgetUID();
      var chart=false;

      function redraw(first_time, silent){
        if(!silent){ $(opts.elem).css('opacity', 0.5); }
        var gauge_values = {};
        var gauges_left = opts.gauges.length;
        var at = parseInt(new Date().getTime()/1000);
        $(opts.gauges).each(function(i,gauge){
          $.ajax({
            url: FnordMetric.p + '/' + currentNamespace+'/gauge/'+gauge+'?at='+at,
            success: function(_resp){
              var resp = JSON.parse(_resp);
              gauges_left -= 1;
              for(_tk in resp){
                gauge_values[gauge] = parseInt(resp[_tk]||0);
              }
              if(gauges_left==0){
                redrawChart(first_time, gauge_values);
              }
            }
          });
        });
      }

      function redrawChart(first_time, gauge_values){

        var series_data = [];

        for(_gkey in gauge_values){
          series_data.push([
            opts.gauge_titles[_gkey],
            gauge_values[_gkey]
          ]);
        }

        chart = new Highcharts.Chart({
          chart: {
            renderTo: 'container-'+widget_uid,
            defaultSeriesType: 'pie',
            height: 270,
            spacingTop: 5,
            spacingBottom: 30
          },
          credits: {
            enabled: false
          },
          title: { text: '' },
          legend: {
            layout: 'horizontal',
            align: 'top',
            verticalAlign: 'top',
            margin: 25,
            borderWidth: 0
          },
          tooltip: {
            formatter: function() {
              return '<b>'+ this.point.name +'</b>: '+ this.y + ' (' + this.percentage.toFixed(1) + '%)';
            }
          },
          plotOptions: {
            pie: {
              animation: first_time,
              dataLabels: {
                formatter: function() {
                  return '<b>'+ this.point.name +'</b><br />'+ this.percentage.toFixed(1) +' %';
                }
              }
            }
          },
          series: [
            {
              name: opts.title,
              data: series_data
            }
          ]
        });

        //chart.redraw();
        $(opts.elem).css('opacity', 1);
      }

      function drawLayout(){
        $(opts.elem).append( $('<div></div>').attr('class', 'headbar small').append(
          $('<span></span>').html(opts.title)
        ) ).append(
          $('<div></div>').attr('id', 'container-'+widget_uid).css({
            height: 270
          })
        );
      }

      drawLayout();
      redraw(true);

      if(opts.autoupdate){
        var autoupdate_interval = window.setInterval(function(){
          redraw(false, true);
        }, opts.autoupdate*1000);

        $('body').bind('fm_dashboard_close', function(){
          window.clearInterval(autoupdate_interval);
        });
      }

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
      $('<div class="headbar small"></div>').html('Event Types')
    ).append(typeListElem);
    var feedElem = $('<div class="sessions_feed"></div>').html(
      $('<div class="headbar small"></div>').html('Event Feed')
    ).append(feedInnerElem);
    var sideElem = $('<div class="sessions_sidebar"></div>').html(
      $('<div class="headbar small"></div>').html('Active Users')
    ).append(listElem);

    var eventsPolledUntil = false;
    var eventsFilter = {uncheckedTypes: [], checkedSessions: []};
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
          url: FnordMetric.p + '/' + currentNamespace+'/sessions',
          success: callbackSessionPoll()
        });
      });
    };

    function loadEventHistory(params){
      feedInnerElem.html('');
      $.ajax({
        url: FnordMetric.p + '/' + currentNamespace+'/events',
        data: params,
        success: function(_data, _status){
          var data = JSON.parse(_data).events;
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
        url: FnordMetric.p + '/' + currentNamespace+'/event_types',
        success: function(_data){
          var data = JSON.parse(_data);
          $(data.types).each(function(i,v){
            if((v.length > 0) && (v.slice(0,5)!='_set_')){
              addEventType(v,v);
            }
          });
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
        $.ajax({
          url: FnordMetric.p + '/' + currentNamespace+'/events?since='+eventsPolledUntil,
          success: callbackEventsPoll()
        });
      });
    };

    function callbackEventsPoll(){
      return (function(_data, _status){
        var data = JSON.parse(_data)
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
      });
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

      event_time.html(formatTimeOfDay(event_data._time));

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
      close: close
    };

  });


  var dashboardView = (function(dashboard_name){

    var widgets = [];
    var viewport = null;

    function load(_viewport){
      viewport = _viewport.html('');
      $.ajax({
        url: FnordMetric.p + '/' + currentNamespace+'/dashboard/'+dashboard_name,
        success: function(resp, status){
          var conf = JSON.parse(resp);
          renderWidgets(conf.widgets);
        }
      });
    };

    function renderWidgets(_widgets){
      for(wkey in _widgets){
        var widget = _widgets[wkey];
        widget["elem"] = $('<div class="widget"></div>');
        widgets[wkey] = widget;
        viewport.append(widget.elem);
        resizeWidget(wkey);
        renderWidget(wkey);
      };
      resize();
    };

    function renderWidget(wkey){
      var widget = widgets[wkey];
      /* argh... */
      if(widget.klass=='TimelineWidget'){ timelineWidget().render(widget); }
      if(widget.klass=='BarsWidget'){ barsWidget().render(widget); }
      if(widget.klass=='NumbersWidget'){ numbersWidget().render(widget); }
      if(widget.klass=='ToplistWidget'){ toplistWidget().render(widget); }
      if(widget.klass=='PieWidget'){ pieWidget().render(widget); }
			if(widget.klass=="HtmlWidget") { htmlWidget().render(widget); }
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
      $('body').trigger('fm_dashboard_close');
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
    p: '',
    renderDashboard: renderDashboard,
    renderSessionView: renderSessionView,
    resizeView: resizeView,
    init: init
  };

})();
