FnordMetric.widgets.numbersWidget = function(){

  var max_per_row = 8;
  var opts;

  function render(_opts){

    opts = _opts;

    opts.elem.append(
      $('<div class="headbar small"></div>').html(opts.title)
    ).css({
      'marginBottom': 20,
      'overflow': 'hidden'
    });

    if(!opts.series_titles){
      opts.series_titles = {};
    }

    for(k in opts.series){

      if (!opts.series_titles[opts.series[k]]){
        opts.series_titles[opts.series[k]] = opts.series[k];
      }

      var container = $('<div></div>')
        .addClass('numbers_container')
        .attr('rel', opts.series[k])
        .append($('<div class="title">').html(opts.series_titles[opts.series[k]]));

      opts.elem.append(container);
    }

    if(opts.autoupdate){
      var secs = parseInt(opts.autoupdate);
      if(secs > 0){

        var autoupdate_interval = window.setInterval(function(){
          requestValues();
        }, secs*1000);

        $('body').bind('fm_dashboard_close', function(){
          window.clearInterval(autoupdate_interval);
        });
      }
    }

    requestValues();
  }

  function resize(){
    $('.numbers_container', opts.elem).each(function(i, e){
      var num_numbers = $('.number', e).length;
      if(num_numbers > max_per_row){ num_numbers = max_per_row; }
      $(e).css('width', (num_numbers * 95)+'px');
    });
  }

  function requestValues(){
    for(k in opts.series){
      FnordMetric.publish({
        "namespace": FnordMetric.currentNamespace,
        "type": "widget_request",
        "klass": "NumbersWidget",
        "channel": opts.channel,
        "cmd": "values_for",
        "gauge": opts.series[k],
        "offsets": opts.offsets,
        "widget_key": opts.widget_key
      })
    }
  }

  function renderValues(series, values){
    for(vkey in values){
      if(!values[vkey].desc){ values[vkey].desc = vkey; }

      var celem = $('.numbers_container[rel="'+series+'"]', opts.elem)
      var velem = $('.number[rel="'+vkey+'"]', celem);
      var vdesc = FnordMetric.util.evilFormat(values[vkey].desc);

      if (velem.length == 0){
        velem = $('<div></div>')
          .addClass('number')
          .attr('rel', vkey)
          .append($('<span class="desc">').html(vdesc))
          .append($('<span class="value">').html(0))
        celem.append(velem);
      }

      if(!!opts.dont_animate){
        velem.attr('data', values[vkey].value) 
        $('.value', velem).html(FnordMetric.util.formatGaugeValue(vkey, values[vkey].value));
      } else {
        velem.attr('data', values[vkey].value)      
      }
    }
    
    resize();

    if(!opts.dont_animate){
      FnordMetric.util.updateNumbers($(opts.elem), 4);
    }
  }

  function announce(ev){
    if(ev.widget_key == opts.widget_key){
      if((ev.class == "widget_response") && (ev.cmd == "values_for")){
        renderValues(ev.series, ev.values)
      }
    }
  }


  return {
    render: render,
    announce: announce
  };

};