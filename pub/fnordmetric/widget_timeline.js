FnordMetric.js('jquery-1.6.1.min.js', function(){  
FnordMetric.js('highcharts/highcharts.js', function(){  
FnordMetric.css('fnordmetric.css', function(){});
FnordMetric.css('widget_timeline.css', function(){});

  $('body').append( $('<div></div>').attr('class', 'headbar').append(
    $('<div></div>').attr('class', 'datepicker')
  ).append(
    $('<div></div>').attr('class', 'button').append($('<span></span>').html('Foobar'))
  ).append(
    $('<div></div>').attr('class', 'button').append($('<span></span>').html('Barfoo'))
  ).append(
    $('<h2></h2>').html(widget_config.title)
  ) );
  $('body').append( $('<div></div>').attr('id', 'container') );


  console.log(widget_config);


  var chart_options = {     
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
  };


  chart = new Highcharts.Chart(chart_options);

  for(n in widget_config.metrics){
    var _query = '?at='+widget_config.start_timestamp+'-'+widget_config.end_timestamp+
                 '&tick='+widget_config.tick+(widget_config.delta ? '&delta=1' : '');
    $.ajax({
      url: '/fnordmetric/metric/'+widget_config.metrics[n]+_query, 
      success: function(json){                
        for(i in json.values){ json.values[i][0] = json.values[i][0]*1000; }
        chart.addSeries({name: widget_config.metrics[n], data: json.values}); 
        chart.redraw();
      }
    });         
  }  


});  
});