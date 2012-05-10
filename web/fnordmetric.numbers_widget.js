FnordMetric.widgets.numbersWidget = function(){

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