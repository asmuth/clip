FnordMetric.widgets.barsWidget = function(){

    var widget_uid = "fnord-" + parseInt(Math.random()*99990000);
    var width, height, opts, graph, gconfig, legend, hoverDetail, shelving, highlighter, values;

    var cardinal = true;

    function render(_opts){
      opts = _opts;

      if(!opts.default_style){ opts.default_style = 'line'; }
      if(!opts.default_cardinal){ opts.default_cardinal = true; }

      draw_layout();
      
      $(opts.elem)
        .addClass('resize_listener')
        .bind('fm_resize', renderChart);

      gconfig = {
        element: $('.container', opts.elem)[0],
        renderer: "bar",
        width: width,
        height: height - 20,
        padding: { top: 0.1, bottom: 0 }
      }

      if(opts.async_chart){
        updateChart();
      } else {
        if(!opts.values){ opts.values = [[0,0],[1,0]]; }
        values = opts.values;
        renderChart();
      }
    }


    function draw_layout(){
      if(!opts.no_headbar){
        $(opts.elem).append(
          $('<div></div>')
            .addClass('headbar')
            .append($('<h2></h2>').html(opts.title))
        );
      }

      $(opts.elem).append(
        $('<div></div>')
          .addClass('container')
          .css({
            height: opts.height,
            margin: '20px 23px 25px 23px',
          })
      );
      
      if(opts.async_chart){
        $('.headbar', opts.elem).prepend(
          $('<div></div>')
            .addClass('button ml')
            .append($('<span></span>').html('&larr;'))
            .click(function(){ moveRange(-1); })
        ).prepend(
          $('<div></div>')
            .addClass('datepicker')
        ).prepend(
          $('<div></div>')
            .addClass('button')
            .append($('<span></span>').html('&rarr;'))
            .click(function(){ moveRange(1); })
        );
      }
    }

    function renderChart(){
      width = opts.elem.width() - 50;
      height = opts.height || 240;

      gconfig.width = width;
      gconfig.height = height;

      $(gconfig.element).html("");

      gconfig.series = [{
        name: opts.title,
        color: opts.color,
        data: []
      }];

      for(var n=0; n < values.length; n++){
        gconfig.series[0].data.push({
          x: n,
          label: values[n][0],
          y: values[n][1]
        });
      }

      graph = new Rickshaw.Graph(gconfig);

      new Rickshaw.Graph.Axis.Y({
        graph: graph,
      }).render();

      graph.configure(gconfig);
      graph.render();
    }


    function announce(evt){
      if(evt.widget_key == opts.widget_key){
        if((evt.class == "widget_response") && (evt.cmd == "values_at")){
          running_request = false;
          $(opts.elem).css('opacity', 1); 
          updateSeriesData(evt.gauges);
        }
      }
    }

    function requestValuesAsync(){
      FnordMetric.publish({
        "type": "widget_request",
        "klass": "TimeseriesWidget",
        "gauges": opts.gauges,
        "cmd": "values_at",
        "tick": opts.tick, 
        "since": opts.start_timestamp,
        "until": opts.end_timestamp,
        "widget_key": opts.widget_key
      })
    }

    function updateSeriesData(dgauges){
      // gconfig.series = [];

      // for(var ind = 0; ind < dgauges.length; ind++){
        
      //   gconfig.series.push({
      //     name: dgauges[ind].key,
      //     color: opts.series[ind].color,
      //     data: []
      //   });
        
      //   for(_time in dgauges[ind].vals){
      //     gconfig.series[ind].data.push(
      //       { x: parseInt(_time), y: parseInt(dgauges[ind].vals[_time] || 0) }
      //     );
      //   }

      // }

      // renderChart();
    }

    function updateChart(first_time, silent){
      if(!silent){ $(opts.elem).css('opacity', 0.5); }    
      updateRange();
      redrawDatepicker();
      requestValuesAsync();
    }

    function redrawDatepicker(){
      // dateFormat('%d.%m.%y %H:%M', parseInt(opts.start_timestamp)*1000) +
      $('.datepicker', opts.elem).html(
        opts.start_timestamp +
        '&nbsp;&dash;&nbsp;' +
        opts.end_timestamp
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

/*
FnordMetric.widgets.barsWidget = function(){

  function render(opts){

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var chart=false;
    var max_y=0;

    function redraw(first_time, silent){
      if(!silent){ $(opts.elem).css('opacity', 0.5); }
      max_y=0;

      var gauge = opts.gauges[0];
      var _url = FnordMetric.p + '/' + FnordMetric.currentNamespace +'/gauge/'+gauge;
      if(opts.tick){ _url += ("?tick=" + opts.tick); }

      $.ajax({
        url: _url,
        success: redrawGauge(first_time, gauge)
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
*/