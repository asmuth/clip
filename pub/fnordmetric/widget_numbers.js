FnordMetric.css('/fnordmetric/fnordmetric.css', function(){});
FnordMetric.js('/jquery-1.6.1.min.js', function(){  

  drawLayout();
  updateDataAndValues();

  window.setInterval(updateDataAndValues, 5000);

  function drawLayout(){
    for(n in widget_config.metrics){
      $('body').append(container=$('<div></div>').attr('class', 'numbers_container').attr('rel', widget_config.metrics[n]));
      container.append($('<div></div>').addClass('title').html(widget_config.metrics[n]));
      for(var k in widget_config.intervals){
        container.append($('<div></div>').addClass('number').attr('rel', k).attr('data',0).append(
          $('<span></span>').addClass('value').html(0)
        ).append(
          $('<span></span>').addClass('desc').html(k)
        ));
      } 
    }
  }

  function updateDataAndValues(){
    for(n in widget_config.metrics){
      for(k in widget_config.intervals){
        var _url = FnordMetric.p+'/metric/'+widget_config.metrics[n]+'?'+widget_config.intervals[k];
        $.get(_url, updateSingleNumber(widget_config.metrics[n], k));
      }
    }
  }

  function updateSingleNumber(metric_name, interval_name){
    return function(json){
      var _elem = $('.number[rel="'+interval_name+'"]', $('.numbers_container[rel="'+metric_name+'"]'));
      _elem.attr('data', json.value);
      updateValues(4);
    };
  }

  function formatValue(value){
    if(value < 10){ return value.toFixed(2); }
    if(value > 1000){ return (value/1000.0).toFixed(1) + "k"; }
    return value.toFixed(0); 
  }

  function updateValues(diff_factor){
    var still_running = false;
    $('.number').each(function(){
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
        $('.value', this).html(formatValue(new_val));
      }
    });
    if(still_running){ 
      (function(df){
        window.setTimeout(function(){ updateValues(df); }, 30);
      })(diff_factor);
    }
  }



});