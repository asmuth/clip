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
            margin: '10px 23px 25px 23px',
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


      if (values.length == 0)
        values.push(["No data yet",1]);

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
        if((evt.class == "widget_response") && (evt.cmd == "values_for")){
          running_request = false;
          $(opts.elem).css('opacity', 1); 
          values = evt.values;
          renderChart();
        }
      }
    }

    function requestValuesAsync(){
      FnordMetric.publish({
        "type": "widget_request",
        "klass": "BarsWidget",
        "gauge": opts.gauge,
        "cmd": "values_for",
        "since": opts.start_timestamp,
        "until": opts.end_timestamp,
        "widget_key": opts.widget_key
      })
    }

    function updateChart(first_time, silent){
      if(!silent){ $(opts.elem).css('opacity', 0.5); }    
      updateRange();
      redrawDatepicker();
      requestValuesAsync();
    }

    function redrawDatepicker(){
      $('.datepicker', opts.elem).html(
        FnordMetric.util.dateFormat(opts.start_timestamp) +
        '&nbsp;&dash;&nbsp;' +
        FnordMetric.util.dateFormat(opts.end_timestamp)
      );
    }

    function updateRange(force){
      if(!opts.tick){
        opts.tick = opts.ticks[0];
      }

      if(!opts.start_timestamp || !opts.end_timestamp || !!force){
        opts.end_timestamp = parseInt(new Date().getTime() / 1000);
        opts.start_timestamp = opts.end_timestamp - opts.tick;
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

