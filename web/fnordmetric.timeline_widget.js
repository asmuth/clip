
FnordMetric.widgets._timelineWidget = function(){

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var width, height, canvas, series, opts, xtick;
    var xticks = 30;
    var running_request = false;
  
    var series_paths = {};
    var series_values = {};

    var xpadding = 30;

    function render(_opts){
      opts = _opts;
      //if(!silent){ $(opts.elem).css('opacity', 0.5); }

      if(!opts.draw_points){
        opts.draw_points = true;
      }

      drawLayout(opts);

      width = opts.elem.width() - (xpadding * 2) - 15;
      height = opts.height || 240;
      xtick = width / (xticks - 1);

      canvas = d3.select('#container-'+widget_uid)
        .append("svg:svg")
        .attr("width", width)
        .attr("height", height);

      canvas.selectAll("*").remove();

      //canvas = Raphael('container-'+widget_uid, width+(2*xpadding), height+30);

      for (ind in opts.series){
        series_values[opts.series[ind]] = {};
      }

      updateRange();
      updateChart();

      //canvas.drawGrid(0, 0, width+(2*xpadding), height, 1, 6, "#ececec");
    }

    function announce(evt){
      if(evt.widget_key == opts.widget_key){
        if((evt._class == "widget_response") && (evt.cmd == "values_at")){
          running_request = false;
          updateSeriesData(evt.tick, evt.values);
          updateChart();
        }
      }
    }

    function requestValues(_tick, times){
      if(times.length > 0){
        if(!running_request){
          running_request = (new Date).getTime();
          requestValuesAsync(_tick, times);
        }
      }
    }

    function requestValuesAsync(_tick, times){
      console.log('request');
      FnordMetric.publish({
        "_class": "widget_request",
        "_channel": opts.channel,
        "cmd": "values_at",
        "tick": _tick, 
        "ticks": times,
        "widget_key": opts.widget_key
      })
    }


    function changeTick(){
      opts.tick = parseInt($(this).attr('data-tick'));
      opts.start_timestamp = null;
      opts.end_timestamp = null;
      updateRange();
      redrawDatepicker();
      updateChart();
    }

    function updateSeriesData(_tick, values){
      for(_series in values){
        for(_t in values[_series]){
          series_values[_series][_tick+"+"+_t] = values[_series][_t];
        }
      }
    }

    function updateChart(){
      var _ticks = [];
      var _miss = [];
      
      for(sind in opts.series){
        var _last = opts.end_timestamp;
        var _delta = (_last -  opts.start_timestamp) / xticks;
        var _sdata = [];

        for(var n=0; n < xticks; n++){
          var _t = (parseInt(_last / opts.tick) * opts.tick);
          var _v = series_values[opts.series[sind]][opts.tick+"+"+_t];
          if((_v === undefined) && (_miss.indexOf(_t) == -1)){ _miss.push(_t); }
          _sdata.push(_v || 0);
          _last -= _delta;
        }

        drawSeries(opts.series[sind], _sdata);
      }

      if(_miss.length > 0)
        requestValues(opts.tick, _miss);

      redrawDatepicker();
    }

    function drawSeries(series, series_data){
      
        //var path_string = "M0,"+height;
        var path_string = "";
        var _max;
        var _color = '0066CC';
        
        if (series_paths[series]){
          for(ind in series_paths[series]){ 
            series_paths[series][ind].remove();
          }
        }

        series_paths[series] = [];

        if(!_max){ _max = Math.max.apply(Math, series_data)*1.1; }
        if(_max < 1){ _max = 1; }

        $(series_data).each(function(i,v){    

          var p_x = xpadding+(i*xtick);
          var p_y = (height-((v/_max)*height));
            
          path_string += ( ( i == 0 ? "M" : "L" ) + p_x + ',' + p_y );

          // if(i%label_mod==0){
          //   canvas.text(p_x, height+10, labels[i]).attr({
          //     font: '10px Helvetica, Arial', 
          //     fill: "#777"
          //   });
          // }

          if(opts.draw_points){
            series_paths[series].push(canvas
              .append("svg:circle")
              .attr("cx", p_x)
              .attr("cy", p_y)
              .attr("stroke-width", "1px")
              .attr("stroke", "#fff")
              .attr("fill", _color)
              .attr("fill-opacity", 1)
              .attr("r", 4)
            );
          }



          // var htrgt = canvas.rect(p_x - 20, p_y - 20, 40, 40).attr({
          //   stroke: "none", 
          //   fill: "#fff", 
          //   opacity: 0
          // }).toFront();

          //series_paths[series].push(htrgt);

          // (function(htrgt, series_paths){

          //   var t_y = p_y + 9;
          //   var ttt = canvas.text(p_x, t_y+10, v).attr({
          //     font: '12px Helvetica, Arial',
          //     fill: "#fff",
          //     opacity: 0
          //   });
            
          //   var tttb = ttt.getBBox();
          //   var ttw = tttb.width+20;
          //   var tt = canvas.rect(p_x-(ttw/2), t_y, ttw, 22, 5).attr({
          //     stroke: "none",
          //     fill: "#000",
          //     opacity: 0
          //   }).toBack();

          //   series_paths[series].push(tt);
          //   series_paths[series].push(ttt);

          //   $(htrgt[0]).hover(function(){
          //     tt.animate({ opacity: 0.8 }, 300);
          //     ttt.animate({ opacity: 0.8 }, 300);
          //   }, function(){
          //     tt.animate({ opacity: 0 }, 300);
          //     ttt.animate({ opacity: 0 }, 300);
          //   });

          // })(htrgt, series_paths);

        });


        series_paths[series].push(canvas.append("svg:path")
          .attr("fill", "none")
          .attr("stroke", "steelblue")
          .attr("stroke-width", 3)
          .attr("d", path_string)
        );

         

        // series_paths[series].push(
        //   canvas.path(path_string).attr({
        //     stroke: _color, 
        //     "stroke-width": 3, 
        //     "stroke-linejoin": 'round'
        //   }).toBack()
        // );

        // path_string += "L"+(width+xpadding)+","+height+" L"+xpadding+","+height+" Z";

        // series_paths[series].push(
        //   canvas.path(path_string).attr({
        //     stroke: "none", 
        //     fill: _color, 
        //     opacity: 0.1
        //   }).toBack()
        // );
    }

    function drawLayout(opts){
      $(opts.elem).append( $('<div></div>').attr('class', 'headbar').append(
        $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('refresh')).click(
          function(){ updateChart(); }
        )
      ).append(
        $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('&rarr;')).click(
          function(){ moveRange(-1); }
        )
      ).append(
        $('<div></div>').attr('class', 'datepicker')
      ).append(
        $('<div></div>').attr('class', 'button ml').append($('<span></span>').html('&larr;')).click(
          function(){ moveRange(1); }
        )
      ).append(
        $('<h2></h2>').html(opts.title)
      ) ).append(
        $('<div></div>').attr('id', 'container-'+widget_uid).css({
          height: opts.height + 6,
          marginBottom: 20,
          overflow: 'hidden'
        })
      );

      if(opts.ticks){
        $('.headbar', opts.elem).append('<div class="tick_btns btn_group"></div>');
        for(__tick in opts.ticks){
          var _tick = opts.ticks[__tick];
          $('.tick_btns', opts.elem).append(
            $('<div></div>').attr('class', 'button tick').append($('<span></span>')
              .html(FnordMetric.util.formatTimeRange(_tick)))
              .attr('data-tick', _tick)
              .click(changeTick)  
          );
        }
      }
    }

    function redrawDatepicker(){
      $('.datepicker', opts.elem).html(
        Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.start_timestamp)*1000) +
        '&nbsp;&dash;&nbsp;' +
        Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.end_timestamp)*1000)
      );
    }

    function updateRange(){
      if(!opts.tick){
        opts.tick = opts.ticks[0];
      }

      if(!opts.start_timestamp || !opts.end_timestamp){
        opts.end_timestamp = parseInt(new Date().getTime() / 1000);
        opts.start_timestamp = opts.end_timestamp - (opts.tick * xticks);
      }


      if(
        (parseInt(new Date().getTime()/1000) - opts.end_timestamp) >
        (opts.include_current ? 0 : opts.tick)
      ){
        opts.end_timestamp += opts.tick;
        opts.start_timestamp += opts.tick;
      }
    }

    function moveRange(direction){
      v = opts.tick*direction*8;
      opts.start_timestamp += v;
      opts.end_timestamp += v;
      updateChart();
    }


    return {
      render: render,
      announce: announce
    }

};














FnordMetric.widgets.timelineWidget = function(){

  function render(opts){

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var chart=false;
    var max_y=0;

    if (!opts.height){ opts.height = 250; }

    function redrawWithRange(first_time, silent){
      if(!silent){ $(opts.elem).css('opacity', 0.5); }

      if(!opts.start_timestamp || !opts.end_timestamp){
        opts.end_timestamp = parseInt(new Date().getTime() / 1000);
        opts.start_timestamp = opts.end_timestamp - (opts.tick * 30);
      }

      redrawDatepicker();

      var _query = '?at='+opts.start_timestamp+'-'+opts.end_timestamp + '&tick=' + opts.tick;
      //chart.series = [];
      max_y=0;
      //metrics_completed = 0;
      $(opts.gauges).each(function(i,gauge){
        $.ajax({
          url: FnordMetric.p + '/' + FnordMetric.currentNamespace +'/gauge/'+gauge+_query,
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

    function updateRange(){
      if(
        (parseInt(new Date().getTime()/1000) - opts.end_timestamp) >
        (opts.include_current ? 0 : opts.tick)
      ){
        opts.end_timestamp += opts.tick;
        opts.start_timestamp += opts.tick;
      }
    }

    function moveRange(direction){
      v = opts.tick*direction*8;
      opts.start_timestamp += v;
      opts.end_timestamp += v;
      redrawWithRange();
    }

    function changeTick(new_tick){
      opts.tick = parseInt($(this).attr('data-tick'));
      opts.start_timestamp = null;
      opts.end_timestamp = null;
      drawChart();
      redrawWithRange(true);
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
        $('<div></div>').attr('class', 'button ml').append($('<span></span>').html('&larr;')).click(
          function(){ moveRange(-1); }
        )
      ).append(
        $('<h2></h2>').html(opts.title)
      ) ).append(
        $('<div></div>').attr('id', 'container-'+widget_uid).css({
          height: opts.height + 6,
          marginBottom: 20,
          overflow: 'hidden'
        })
      );

      if(opts.ticks){
        $('.headbar', opts.elem).append('<div class="tick_btns btn_group"></div>');
        for(__tick in opts.ticks){
          var _tick = opts.ticks[__tick];
          $('.tick_btns', opts.elem).append(
            $('<div></div>').attr('class', 'button tick').append($('<span></span>')
              .html(FnordMetric.util.formatTimeRange(_tick)))
              .attr('data-tick', _tick)
              .click(changeTick)  
          );
        }
      }
    }

    function drawChart(){
      chart = new Highcharts.Chart({
        chart: {
          renderTo: 'container-'+widget_uid,
          defaultSeriesType: opts.plot_style,
          height: opts.height + 20
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

    updateRange();
    drawLayout();
    drawChart();

    redrawWithRange(true);

    if(opts.autoupdate){
      var secs = parseInt(opts.autoupdate);
      if(secs > 0){

        var autoupdate_interval = window.setInterval(function(){
          updateRange();
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