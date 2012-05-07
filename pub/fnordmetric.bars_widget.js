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