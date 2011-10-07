FnordMetric.js('jquery-1.6.1.min.js', function(){  
FnordMetric.js('highcharts/highcharts.js', function(){  
FnordMetric.css('fnordmetric.css', function(){});
FnordMetric.css('widget_graph.css', function(){});

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


  chart = new Highcharts.Chart({     
    chart: { renderTo: 'container', defaultSeriesType: widget_config.chart_type, height: 270 },
    series: widget_config.series,
    title: { text: '' },
    xAxis: { 
      categories: widget_config.x_labels, 
      title: (widget_config.x_title||''), 
      labels: { step: parseInt(widget_config.x_labels.length/17) } 
    },
    yAxis: { 
      categories: widget_config.y_labels, 
      title: (widget_config.y_title||''), 
      maxPadding: 0 
    },
    
    /*yAxis: {
      title: { text: 'Fnord' },
      plotLines: [{ value: 0, width: 1, color: '#808080' }]
    },*/

    legend: {
      layout: 'horizontal',
      align: 'top',
      verticalAlign: 'top',
      x: -5,
      y: -3,
      margin: 25,
      borderWidth: 0
    },

    tooltip: {
      formatter: function(){
        return '<b>'+ this.series.name +'</b><br/>'+
        this.x +': '+ this.y +'°';
      }
    },

  });
    
});  
});