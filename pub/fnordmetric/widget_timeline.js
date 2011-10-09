FnordMetric.js('jquery-1.6.1.min.js', function(){  
FnordMetric.js('highcharts/highcharts.js', function(){  
FnordMetric.css('fnordmetric.css', function(){});
FnordMetric.css('widget_timeline.css', function(){});


  drawLayout();
  chart = new Highcharts.Chart({     
    chart: { renderTo: 'container', defaultSeriesType: widget_config.chart_type, height: 270 },
    series: [],
    title: { text: '' },
    xAxis: {       
      type: 'datetime',
      tickInterval: widget_config.tick * 1000, 
      title: (widget_config.x_title||''), 
      labels: { step: 2 } 
    },
    yAxis: { 
      title: (widget_config.y_title||''), 
      maxPadding: 0 
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
  });
  redrawWithRange(true);

  function redrawWithRange(first_time){
    $("#container").css('opacity', 0.5);
    redrawDatepicker();    
    var _query = '?at='+widget_config.start_timestamp+'-'+widget_config.end_timestamp+
                 '&tick='+widget_config.tick+(widget_config.delta ? '&delta=1' : '');    
    chart.series = [];
    metrics_completed = 0;
    for(n in widget_config.metrics){
      $.ajax({
        url: '/fnordmetric/metric/'+widget_config.metrics[n]+_query, 
        success: redrawMetric(first_time, n)
      });         
    }  
  }

  function redrawMetric(first_time, n){
    return (function(json){                   
      for(i in json.values){ json.values[i][0] = json.values[i][0]*1000; }
      if(!first_time){ 
        chart.get('series-'+n).setData(json.values);
      } else {
        chart.addSeries({name: widget_config.metrics[n], data: json.values, id: 'series-'+n });     
      }       
      if((metrics_completed += 1) == widget_config.metrics.length){ 
        $("#container").css('opacity', 1);
      }
    });
  }

  function redrawDatepicker(){
    $('.datepicker').html(
      Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(widget_config.start_timestamp)*1000) + 
      '&nbsp;&dash;&nbsp;' +
      Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(widget_config.end_timestamp)*1000) 
    );
  }

  function moveRange(direction){
    v = widget_config.tick*direction*8;
    widget_config.start_timestamp += v;
    widget_config.end_timestamp += v;
    redrawWithRange();
  }
  
  function drawLayout(){
    $('body').append( $('<div></div>').attr('class', 'headbar').append(
      $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('refresh')).click(
        function(){ redrawWithRange(); }
      )
    ).append(
    //  $('<div></div>').attr('class', 'button').append($('<span></span>').html('1h'))
    //).append(
    //  $('<div></div>').attr('class', 'button').append($('<span></span>').html('1d'))
    //).append(
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
      $('<h2></h2>').html(widget_config.title)
    ) );
    $('body').append( $('<div></div>').attr('id', 'container') );
  }

});  
});