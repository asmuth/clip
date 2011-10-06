js_loader.js('/jquery-1.6.1.min.js', function(){  
js_loader.js('/highcharts/highcharts.js', function(){  

  $('body').append( $('<div></div>').attr('id', 'container') );

  chart = new Highcharts.Chart({ 
    
    chart: {
      renderTo: 'container',
      defaultSeriesType: 'line'
    },

    series: widget_config.series,

    title: { text: widget_config.title },
    subtitle: { text: widget_config.title },

    xAxis: {
      categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
    },
    yAxis: {
      title: { text: 'Fnord' },
      plotLines: [{ value: 0, width: 1, color: '#808080' }]
    },

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