function renderWidget(conf){ 
  console.log(conf);
  $('body').append( $('<div></div>').attr('id', 'container') );
  chart = new Highcharts.Chart({
      chart: {
         renderTo: 'container',
         defaultSeriesType: 'line',
         marginRight: 130,
         marginBottom: 25
      },
      title: {
         text: conf.title,
         x: -20 //center
      },
      subtitle: {
         text: conf.title,
         x: -20
      },
      xAxis: {
         categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 
            'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
      },
      yAxis: {
         title: {
            text: 'Temperature (Â°C)'
         },
         plotLines: [{
            value: 0,
            width: 1,
            color: '#808080'
         }]
      },
      tooltip: {
         formatter: function() {
                   return '<b>'+ this.series.name +'</b><br/>'+
               this.x +': '+ this.y +'Â°C';
         }
      },
      legend: {
         layout: 'vertical',
         align: 'right',
         verticalAlign: 'top',
         x: -10,
         y: 100,
         borderWidth: 0
      },
      series: conf.series
   });
}