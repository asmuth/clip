FnordMetric.widgets.pieWidget = function(){

  function render(opts){

    return drawLayout();

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var chart=false;

    function redraw(first_time, silent){
      if(!silent){ $(opts.elem).css('opacity', 0.5); }
      var gauge_values = {};
      var gauges_left = opts.gauges.length;
      var at = parseInt(new Date().getTime()/1000);
      $(opts.gauges).each(function(i,gauge){
        $.ajax({
          url: FnordMetric.p + '/' + FnordMetric.currentNamespace+'/gauge/'+gauge+'?at='+at,
          success: function(_resp){
            var resp = JSON.parse(_resp);
            gauges_left -= 1;
            for(_tk in resp){
              gauge_values[gauge] = parseInt(resp[_tk]||0);
            }
            if(gauges_left==0){
              redrawChart(first_time, gauge_values);
            }
          }
        });
      });
    }

    function redrawChart(first_time, gauge_values){

      var series_data = [];

      for(_gkey in gauge_values){
        series_data.push([
          opts.gauge_titles[_gkey],
          gauge_values[_gkey]
        ]);
      }

      /*chart = new Highcharts.Chart({
        chart: {
          renderTo: 'container-'+widget_uid,
          defaultSeriesType: 'pie',
          height: 270,
          spacingTop: 5,
          spacingBottom: 30
        },
        credits: {
          enabled: false
        },
        title: { text: '' },
        legend: {
          layout: 'horizontal',
          align: 'top',
          verticalAlign: 'top',
          margin: 25,
          borderWidth: 0
        },
        tooltip: {
          formatter: function() {
            return '<b>'+ this.point.name +'</b>: '+ this.y + ' (' + this.percentage.toFixed(1) + '%)';
          }
        },
        plotOptions: {
          pie: {
            animation: first_time,
            dataLabels: {
              formatter: function() {
                return '<b>'+ this.point.name +'</b><br />'+ this.percentage.toFixed(1) +' %';
              }
            }
          }
        },
        series: [
          {
            name: opts.title,
            data: series_data
          }
        ]
      });*/

      //chart.redraw();
      $(opts.elem).css('opacity', 1);
    }

    function drawLayout(){
      $(opts.elem).append( $('<div></div>').attr('class', 'headbar small').append(
        $('<span></span>').html(opts.title)
      ) ).append(
        $('<div></div>').attr('id', 'container-'+widget_uid).css({
          height: 270
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
