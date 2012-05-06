FnordMetric.widgets.timelineWidget = function(){

  function render(opts){

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var chart=false;
    var max_y=0;

    if (!opts.height){ opts.height = 250; }

    function redrawWithRange(first_time, silent){
      if(!silent){ $(opts.elem).css('opacity', 0.5); }

      if(!opts.start_timestamp || !opts.end_timestamp){
        opts.end_timestamp = parseInt(new Date().getTime() / 1000);
        opts.start_timestamp = opts.end_timestamp - (opts.tick * 30);
      }

      redrawDatepicker();

      var _query = '?at='+opts.start_timestamp+'-'+opts.end_timestamp + '&tick=' + opts.tick;
      //chart.series = [];
      max_y=0;
      //metrics_completed = 0;
      $(opts.gauges).each(function(i,gauge){
        $.ajax({
          url: FnordMetric.p + '/' + FnordMetric.currentNamespace +'/gauge/'+gauge+_query,
          success: redrawGauge(first_time, gauge)
        });
      });
    }

    function redrawGauge(first_time, gauge){
      return (function(json){
        var raw_data = JSON.parse(json);
        var series_data = [];

        for(p in raw_data){
          series_data.push([parseInt(p)*1000, raw_data[p]||0]);
          max_y = Math.max(max_y, raw_data[p]);
        }

        if(!first_time){
          chart.get('series-'+gauge).setData(series_data);
        } else {
          chart.addSeries({
            name: opts.gauge_titles[gauge],
            data: series_data,
            id: 'series-'+gauge
          });
        }

        chart.yAxis[0].setExtremes(0,max_y);
        chart.redraw();

        // shown on the *first* gauge load
        $(opts.elem).css('opacity', 1);
      });
    }

    function redrawDatepicker(){
      $('.datepicker', opts.elem).html(
        Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.start_timestamp)*1000) +
        '&nbsp;&dash;&nbsp;' +
        Highcharts.dateFormat('%d.%m.%y %H:%M', parseInt(opts.end_timestamp)*1000)
      );
    }

    function updateRange(){
      if(
        (parseInt(new Date().getTime()/1000) - opts.end_timestamp) >
        (opts.include_current ? 0 : opts.tick)
      ){
        opts.end_timestamp += opts.tick;
        opts.start_timestamp += opts.tick;
      }
    }

    function moveRange(direction){
      v = opts.tick*direction*8;
      opts.start_timestamp += v;
      opts.end_timestamp += v;
      redrawWithRange();
    }

    function changeTick(new_tick){
      opts.tick = parseInt($(this).attr('data-tick'));
      opts.start_timestamp = null;
      opts.end_timestamp = null;
      drawChart();
      redrawWithRange(true);
    }

    function drawLayout(){
      $(opts.elem).append( $('<div></div>').attr('class', 'headbar').append(
        $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('refresh')).click(
          function(){ redrawWithRange(); }
        )
      ).append(
        $('<div></div>').attr('class', 'button mr').append($('<span></span>').html('&rarr;')).click(
          function(){ moveRange(1); }
        )
      ).append(
        $('<div></div>').attr('class', 'datepicker')
      ).append(
        $('<div></div>').attr('class', 'button ml').append($('<span></span>').html('&larr;')).click(
          function(){ moveRange(-1); }
        )
      ).append(
        $('<h2></h2>').html(opts.title)
      ) ).append(
        $('<div></div>').attr('id', 'container-'+widget_uid).css({
          height: opts.height + 6,
          marginBottom: 20,
          overflow: 'hidden'
        })
      );

      if(opts.ticks){
        for(__tick in opts.ticks){
          var _tick = opts.ticks[__tick];
          $('.headbar', opts.elem).append(
            $('<div></div>').attr('class', 'button tick').append($('<span></span>')
              .html(FnordMetric.util.formatTimeRange(_tick)))
              .attr('data-tick', _tick)
              .click(changeTick)  
          );
        }
      }
    }

    function drawChart(){
      chart = new Highcharts.Chart({
        chart: {
          renderTo: 'container-'+widget_uid,
          defaultSeriesType: opts.plot_style,
          height: opts.height + 20
        },
        series: [],
        title: { text: '' },
        xAxis: {
          type: 'datetime',
          tickInterval: opts.tick * 1000,
          title: (opts.x_title||''),
          labels: { step: 2 }
        },
        yAxis: {
          title: (opts.y_title||''),
          min: 0,
          max: 1000
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
          line: {
            shadow: false,
            lineWidth: 3
          }
        }
      });
    }

    updateRange();
    drawLayout();
    drawChart();

    redrawWithRange(true);

    if(opts.autoupdate){
      var secs = parseInt(opts.autoupdate);
      if(secs > 0){

        var autoupdate_interval = window.setInterval(function(){
          updateRange();
          redrawWithRange(false, true);
        }, secs*1000);

        $('body').bind('fm_dashboard_close', function(){
          window.clearInterval(autoupdate_interval);
        });

      }
    };

  }

  return {
    render: render
  };

};