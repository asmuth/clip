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
    chart: { renderTo: 'container', defaultSeriesType: 'line' },
    series: widget_config.series,
    title: { text: widget_config.title },
    subtitle: { text: widget_config.title },
    xAxis: { categories: widget_config.x_labels },
    
    /*yAxis: {
      title: { text: 'Fnord' },
      plotLines: [{ value: 0, width: 1, color: '#808080' }]
    },*/

    legend: {
      layout: 'vertical',
      align: 'right',
      verticalAlign: 'top',
      x: -10,
      y: 100,
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