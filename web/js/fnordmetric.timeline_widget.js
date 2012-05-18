FnordMetric.widgets.timelineWidget = function(){

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var width, height, canvas, series, opts, xtick;
    var xticks = 100;
    var running_request = false;
  
    var series_paths = {};
    var series_values = {};

    var xpadding = 30;

    function render(_opts){
      opts = _opts;
      //if(!silent){ $(opts.elem).css('opacity', 0.5); }

      if(opts.xticks){ xticks = opts.xticks; }
      if(!opts.draw_points){ opts.draw_points = true; }
      if(!opts.draw_path){ opts.draw_path = true; }
      if(!opts.draw_ygrid){ opts.draw_ygrid = true; }
      if(!opts.draw_xgrid){ opts.draw_xgrid = true; }

      drawLayout(opts);

      width = opts.elem.width() - (xpadding * 2) - 15;
      height = opts.height || 240;
      xtick = width / (xticks - 1);

      canvas = d3.select('#container-'+widget_uid)
        .append("svg:svg")
        .attr("width", width+(2*xpadding))
        .attr("height", height+30);

      canvas.selectAll("*").remove();

      for (ind in opts.series){
        series_values[opts.series[ind]] = {};
      }

      updateRange();
      updateChart();

      if(opts.autoupdate){
        var secs = parseInt(opts.autoupdate);
        if(secs > 0){
          var autoupdate_interval = window.setInterval(function(){
            updateRange(true); updateChart(); // FIXPAUL: only update if not scrolled
          }, secs*1000);

          $('body').bind('fm_dashboard_close', function(){
            window.clearInterval(autoupdate_interval);
          });
        }
      }
      
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
      var _max = [];
      var _rndr = [];
      
      for(sind in opts.series){
        var _last = opts.start_timestamp + opts.tick;
        var _delta = (opts.end_timestamp - _last) / xticks;
        var _sdata = [];

        for(var n=0; n < xticks; n++){
          var _t = (parseInt(_last / opts.tick) * opts.tick);
          var _v = series_values[opts.series[sind]][opts.tick+"+"+_t];
          if((_v === undefined) && (_miss.indexOf(_t) == -1)){ _miss.push(_t); }
          _sdata.push(_v || 0);
          _last += _delta;
        }

        _max.push(Math.max.apply(Math, _sdata));       
        _rndr.push([opts.series[sind], _sdata]);
      }

      _max = Math.max.apply(Math, _max)*1.1;
      if(_max == 0){ _max = 1; }

      for(rind in _rndr){
        drawSeries(_rndr[rind][0], _rndr[rind][1], _max);
      }

      if(_miss.length > 0)
        requestValues(opts.tick, _miss);

      redrawDatepicker();
    }

    function refreshChart(){
      for(_series in series_values){
        series_values[_series] = {};
      }

      updateRange();
      updateChart();
    }

    function drawSeries(series, series_data, _max){
      
        //var path_string = "M0,"+height;
        var path_string = "";
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

        if(opts.draw_path){
          series_paths[series].push(canvas.append("svg:path")
            .attr("fill", "none")
            .attr("stroke", "steelblue")
            .attr("stroke-width", 3)
            .attr("d", path_string)
          );
        }

        if(opts.draw_area){
          // path_string += "L"+(width+xpadding)+","+height+" L"+xpadding+","+height+" Z";

          // series_paths[series].push(
          //   canvas.path(path_string).attr({
          //     stroke: "none", 
          //     fill: _color, 
          //     opacity: 0.1
          //   }).toBack()
          // );
        }
    }

    function drawLayout(opts){
      $(opts.elem).append( $('<div></div>').attr('class', 'headbar').append(
        $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('refresh')).click(
          function(){ refreshChart(); }
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

    function redrawDatepicker(){
      $('.datepicker', opts.elem).html(
        Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.start_timestamp)*1000) +
        '&nbsp;&dash;&nbsp;' +
        Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.end_timestamp)*1000)
      );
    }

    function updateRange(force){
      if(!opts.tick){
        opts.tick = opts.ticks[0];
      }

      if(!opts.start_timestamp || !opts.end_timestamp || !!force){
        opts.end_timestamp = parseInt(new Date().getTime() / 1000);
        opts.start_timestamp = opts.end_timestamp - (opts.tick * xticks);
      }
    }

    function moveRange(direction){
      v = opts.tick*direction*8;

      if(((opts.end_timestamp + v)*1000) < new Date().getTime()){
        opts.start_timestamp += v;
        opts.end_timestamp += v;  
      }

      updateChart();
    }


    return {
      render: render,
      announce: announce
    }

};
