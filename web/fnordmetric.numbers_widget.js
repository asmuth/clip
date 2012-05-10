FnordMetric.widgets._numbersWidget = function(){

  var max_per_row = 6;
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
        "_class": "widget_request",
        "_channel": opts.channel,
        "cmd": "values_for",
        "series": opts.series[k],
        "widget_key": opts.widget_key
      })
    }
  }

  function renderValues(series, values){
    for(vkey in values){
      if(!values[vkey].desc){ values[vkey].desc = vkey; }

      var celem = $('.numbers_container[rel="'+series+'"]', opts.elem)
      var velem = $('.number[rel="'+vkey+'"]', celem);

      if (velem.length == 0){
        velem = $('<div></div>')
          .addClass('number')
          .attr('rel', vkey)
          .append($('<span class="desc">').html(values[vkey].desc))
          .append($('<span class="value">').html(0))
        celem.append(velem);
      }

      velem.attr('data', values[vkey].value)      
    }
    resize();
    updateDisplay(4);
  }

  function announce(ev){
    if(ev.widget_key == opts.widget_key){
      if((ev._class == "widget_response") && (ev.cmd == "values_for")){
        renderValues(ev.series, ev.values)
      }
    }
  }

  function updateDisplay(diff_factor){
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
        window.setTimeout(function(){ updateDisplay(df); }, 30);
      })(diff_factor);
    }
  }

  return {
    render: render,
    announce: announce
  };

};