var FnordMetric = (function(){

  var canvasElem = false;
  var currentView = false;

 var toplistWidget = function(){

    function render(opts){

      var current_gauge = false;

      var headbar = $('<div class="headbar"></div>').append(
        $('<h2></h2>').html(opts.title)
      );

      opts.elem.append(headbar).css({
        'marginBottom': 20,
        'overflow': 'hidden'
      }).append(
        $('<div class="toplist_inner"></div>')
      );

      var first = true;
      for(k in opts.gauges){
        headbar.append(
          $('<div></div>')
            .attr('class', 'button mr')
            .attr('rel', k)
            .append(
              $('<span></span>').html(opts.gauges[k].title)
            ).click(function(){
              loadGauge($(this).attr('rel'));
            }
          )
        );
        if(first){
          first = false;
          loadGauge(k);
        }
      }

      if(opts.autoupdate){
        var secs = parseInt(opts.autoupdate);
        if(secs > 0){

          var autoupdate_interval = window.setInterval(function(){
            loadGauge(false, true);
          }, secs*1000);

          $('body').bind('fm_dashboard_close', function(){
            window.clearInterval(autoupdate_interval);
          });
        }
      };

      function loadGauge(gkey, silent){
        if(!gkey){ gkey = current_gauge; }
        current_gauge = gkey;
        if(!silent){ $('.toplist_inner', opts.elem).addClass('loading'); }
        var _url = FnordMetric.p + '/' + FnordMetric.currentNamespace + '/gauge/' + gkey;
        $.get(_url, function(_resp){
          var resp = JSON.parse(_resp);
          renderGauge(gkey, resp);
        })
      }

      function renderGauge(gkey, gdata){
        var _elem = $('.toplist_inner', opts.elem).removeClass('loading').html('');
        $(gdata.values).each(function(n, _gd){
          var _perc  = (parseInt(gdata.values[n][1]) / parseFloat(gdata.count))*100;
          var _item = $('<div class="toplist_item"><div class="title"></div><div class="value"></div><div class="percent"></div></div>');
          $('.title', _item).html(gdata.values[n][0]);
          $('.value', _item).html(FnordMetric.util.formatGaugeValue(gkey, parseInt(gdata.values[n][1])));
          $('.percent', _item).html(_perc.toFixed(1) + '%');
          _elem.append(_item);
        });
      }

    }


    return {
      render: render
    };

  };

	var htmlWidget = function(){
		function render(opts){

      opts.elem.append(
        $('<div class="headbar small"></div>').html(opts.title)
      ).css({
        'marginBottom': 20,
        'overflow': 'hidden'
      });
			
			var container = $('<div></div>')
        .addClass('html_container')
				.html(opts.html);
			
			opts.elem.append(container);
		}
		
		function updateValues(opts){}
		function updatedisplay(opts, diff_factor){}
	
		return {
			render: render
		};
	}

  var numbersWidget = function(){


    function render(opts){

      opts.elem.append(
        $('<div class="headbar small"></div>').html(opts.title)
      ).css({
        'marginBottom': 20,
        'overflow': 'hidden'
      });

      for(k in opts.gauges){
        var gtick = parseInt(opts.gauges[k].tick);
        var gtitle = opts.gauges[k].title;

        var container = $('<div></div>')
          .addClass('numbers_container')
          .addClass('size_'+opts.offsets.length)
          .attr('rel', k)
          .append(
            $('<div></div>')
              .addClass('title')
              .html(gtitle)
          );


        $(opts.offsets).each(function(n, offset){
          var _off, _nextoff, _sum;
          if (offset[0]=="s"){
            _off = 0;
            _sum = _nextoff = (gtick * parseInt(offset.slice(1)));
          } else {
            _sum = 0;
            _off = offset*gtick;
            _nextoff = gtick;
          }
          container.append(
            $('<div></div>')
              .addClass('number')
              .attr('rel', k)
              .attr('data-offset', _off)
              .attr('data-sum', _sum)
              .attr('data',0)
              .append(
                $('<span></span>').addClass('desc').html(FnordMetric.util.formatOffset(_off, _nextoff))
              )
              .append(
                $('<span></span>').addClass('value').html(0)
              )
            );
        });

        opts.elem.append(container);
      }

      if(opts.autoupdate){
        var secs = parseInt(opts.autoupdate);
        if(secs > 0){

          var autoupdate_interval = window.setInterval(function(){
            updateValues(opts);
          }, secs*1000);

          $('body').bind('fm_dashboard_close', function(){
            window.clearInterval(autoupdate_interval);
          });

        }

      };

      updateValues(opts);

    }

    function updateValues(opts){
      var values = $('.number', $(opts.elem));
      var values_pending = values.length;
      values.each(function(){
        var _sum = parseInt($(this).attr('data-sum'));
        var num = this;
        var at = parseInt(new Date().getTime()/1000);
        var url = FnordMetric.p + '/' + FnordMetric.currentNamespace + '/gauge/' + $(this).attr('rel');
        if(_sum > 0){
          url += '?at='+(at-_sum)+'-'+at+'&sum=true';
        } else {
          at -= parseInt($(this).attr('data-offset'));
          url += '?at='+at;
        }

        $.get(url, function(_resp){
          var resp = JSON.parse(_resp);
          for(_k in resp){
            $(num).attr('data', (resp[_k]||0));
          }
          if((values_pending -= 1)==0){
            updateDisplay(opts, 4);
          }
        });
      });
    }

    function updateDisplay(opts, diff_factor){
      var still_running = false;
      $('.number', $(opts.elem)).each(function(){
        var target_val = parseFloat($(this).attr('data'));
        var current_val = parseFloat($(this).attr('data-current'));
        if(!current_val){ current_val=0; }
        var diff = (target_val-current_val)/diff_factor;
        if(diff < 1){ diff=1; }
        if(target_val > current_val){
          still_running = true;
          var new_val = current_val+diff;
          if(new_val > target_val){ new_val = target_val; }
          $(this).attr('data-current', new_val);
          $('.value', this).html(FnordMetric.util.formatGaugeValue($(this).attr('rel'), new_val));
        }
      });
      if(still_running){
        (function(df){
          window.setTimeout(function(){ updateDisplay(opts, df); }, 30);
        })(diff_factor);
      }
    }

    return {
      render: render
    };

  };


  var barsWidget = function(){

    function render(opts){

      var widget_uid = FnordMetric.util.getNextWidgetUID();
      var chart=false;
      var max_y=0;

      function redraw(first_time, silent){
        if(!silent){ $(opts.elem).css('opacity', 0.5); }
        max_y=0;
        $(opts.gauges).each(function(i,gauge){
          $.ajax({
            url: FnordMetric.p + '/' + FnordMetric.currentNamespace +'/gauge/'+gauge,
            success: redrawGauge(first_time, gauge)
          });
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

  var pieWidget = function(){

    function render(opts){

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

        chart = new Highcharts.Chart({
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
        });

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


  function renderDashboard(_dash){
    loadView(FnordMetric.views.dashboardView(_dash));
  };

  function renderSessionView(){
    loadView(FnordMetric.views.sessionView());
  }

  function loadView(_view){
    if(currentView){ currentView.close(); }
    canvasElem.html('loading!');
    currentView = _view;
    currentView.load(canvasElem);
    resizeView();
  };

  function resizeView(){
    currentView.resize(
      canvasElem.innerWidth(),
      canvasElem.innerHeight()
    );
  };

  function init(_namespace, _canvasElem){
    canvasElem = _canvasElem;
    FnordMetric.currentNamespace = _namespace;
    loadView(FnordMetric.views.sessionView());
  };

  return {
    renderDashboard: renderDashboard,
    renderSessionView: renderSessionView,
    resizeView: resizeView,
    init: init,
    p: '',
    currentNamespace: false,
    currentWidgetUID: 23,
    ui: {},
    views: {},
    widgets: {},
    util: {}
  };

})();
