FnordMetric.util.getNextWidgetUID = function(){
  return (FnordMetric.currentWidgetUID += 1);
}

FnordMetric.util.decPrint = function(val){
  return (val < 10 ? '0'+val : val);
}

FnordMetric.util.formatTimeOfDay = function(_time){
  if(_time === null) {
    return "";
  } else {
    var time = new Date();
    time.setTime(_time*1000);
    return FnordMetric.util.decPrint(time.getHours()) + ':' +
           FnordMetric.util.decPrint(time.getMinutes()) + ':' +
           FnordMetric.util.decPrint(time.getSeconds());
  }
}

FnordMetric.util.formatTimeRange = function(range){
  if (range < 60){
    return parseInt(range) + ' sec';
  } else if(range<3600){
    return parseInt(range/60) + ' min';
  } else if(range==3600){
    return '1 hour';
  } else if(range<(3600*24)){
    return parseInt(range/3600) + ' hours';
  } else if(range==(3600*24)){
    return '1 day';
  } else {
    return parseInt(range/(3600*24)) + ' days';
  }
}

FnordMetric.util.formatTimeRangePre = function(range, offset){
  if(!offset){ offset=0; } else { offset=parseInt(offset); }
  if((offset == 0) && (range==(3600*24))){
    return 'Today';
  } else if((offset==-1) && (range==3600*24)){
    return 'Yesterday';
  } else if((offset == 0) && (range==3600)){
    return 'This hour';
  } else if((offset==-1) && (range==3600)){
    return 'Last hour';
  } else if((offset == 0) && (range==60)){
    return 'This minute';
  } else if((offset==-1) && (range==60)){
    return 'Last minute';
  } else if(offset==0) {
    return "This " + FnordMetric.util.formatTimeRange(range);
  } else if(offset==-1) {
    return "Last " + FnordMetric.util.formatTimeRange(range);
  } else{
    return FnordMetric.util.formatTimeRange(range) + " (-" + 
    FnordMetric.util.formatTimeRange(range * (offset*-1)) + ")";
  }
}

FnordMetric.util.formatTimeSince = function(time){
  var now = new Date().getTime()/1000;
  var since = now - time;
  return FnordMetric.util.formatTimeRange(since);
}

FnordMetric.util.formatOffset = function(offset, next_offset){
  if((offset == 0) && (next_offset==(3600*24))){
    return 'Today';
  } if((offset == 0) && (next_offset==3600)){
    return 'This hour';
  } else if(offset == 0){
    return 'Last ' + FnordMetric.util.formatTimeRange(next_offset||0);
  } else if(offset==(3600*24)){
    return 'Yesterday';
  } else if(offset==3600){
    return 'Last hour';
  } else {
    return FnordMetric.util.formatTimeRange(offset) + ' ago';
  }
}

FnordMetric.util.formatValue = function(value){
  if(value < 10){
    return value.toFixed(2);
  }
  if(value < 100){
    return value.toFixed(1);
  } else if(value > 1000){
    return (value/1000.0).toFixed(1) + "k";
  } else {
    return value.toFixed(0);
  }
}

FnordMetric.util.formatTimeValue = function(value){
  if (value < 60){
    return parseFloat(value).toFixed(1) + 's';
  } else if(value<3600){
    return parseFloat(value/60).toFixed(1) + 'm';
  } else if(value<(3600*24)){
    return parseFloat(value/3600).toFixed(1) + 'h';
  } else {
    return parseFloat(value/(3600*24)).toFixed(1) + 'd';
  }
}

FnordMetric.util.formatPercentValue = function(value){
  value = parseFloat(value);
  if(value < 10){
    return value.toFixed(2) + '%';    
  } else if(value < 100){
    return value.toFixed(1) + '%';    
  } else {
    return '100%';
  }
}

FnordMetric.util.formatGaugeValue = function(gauge_key, value){
  if(!gauge_key){ gauge_key = ""; }
  if(gauge_key.slice(0,8) === '__time__'){
    return FnordMetric.util.formatTimeValue(value);
  } else if(gauge_key.slice(0,11) === '__percent__'){
    return FnordMetric.util.formatPercentValue(value);
  } else {
    return FnordMetric.util.formatValue(value);
  }
}

FnordMetric.util.updateNumbers = function(trgt_elem, diff_factor){
  var still_running = false;
  if(!diff_factor){ diff_factor = 4; }
  $('.number', $(trgt_elem)).each(function(){
    var target_val = parseFloat($(this).attr('data'));
    var current_val = parseFloat($(this).attr('data-current'));
    if(!current_val){ current_val=0; }
    if(!target_val){ target_val=0; }
    var diff = (target_val-current_val)/diff_factor;
    if((diff > 0) && (diff < 1)){ diff=1; }
    if((diff < 0) && (diff > -1)){ diff=-1; }
    if(target_val != current_val){
      still_running = true;
      var new_val = current_val+diff;
      if((diff > 0) && (new_val > target_val)){ new_val = target_val; }
      if((diff < 0) && (new_val < target_val)){ new_val = target_val; }
      $(this).attr('data-current', new_val);
      $('.value', this).html(FnordMetric.util.formatGaugeValue($(this).attr('rel'), new_val));
    }
  });
  if(still_running){
    (function(te, df){
      window.setTimeout(function(){ 
        FnordMetric.util.updateNumbers(te, df); 
      }, 30);
    })(trgt_elem, diff_factor);
  }
}

FnordMetric.util.dateFormat = function(timestamp){
  var t = new Date(timestamp*1000);
  return FnordMetric.util.decPrint(t.getDate())      + "." + 
         FnordMetric.util.decPrint((t.getMonth()+1)) + "." + 
         FnordMetric.util.decPrint(t.getFullYear())  + " " +
         FnordMetric.util.decPrint(t.getHours())     + ":" +
         FnordMetric.util.decPrint(t.getMinutes())   + " ";
}

FnordMetric.util.format = function(elem){
  $('.ui_trend, .ui_value', $(elem)).each(function(){

    if($(this).attr('data-value')){
      $(this).html(FnordMetric.util.formatGaugeValue('', parseFloat($(this).attr('data-value'))));
    }

    else if($(this).attr('data-percent')){
      $(this).html(FnordMetric.util.formatGaugeValue('__percent__', parseFloat($(this).attr('data-percent'))));
    }

    else if($(this).attr('data-time')){
      $(this).html(FnordMetric.util.formatGaugeValue('__time__', parseFloat($(this).attr('data-time'))));
    }

    else if($(this).attr('data-euro')){
      $(this).html(FnordMetric.util.formatGaugeValue('__euro__', parseFloat($(this).attr('data-euro'))));
    }

    else if($(this).attr('data-dollar')){
      $(this).html(FnordMetric.util.formatGaugeValue('__dollar__', parseFloat($(this).attr('data-dolar'))));
    }

    else if($(this).attr('data-timerange-offset')){
      $(this).html(FnordMetric.util.formatTimeRangePre(
        parseInt($(this).attr('data-timerange-offset')),
        $(this).attr('data-offset')
      ));
    }
    else if($(this).attr('data-timerange')){
      $(this).html(FnordMetric.util.formatTimeRange(parseInt($(this).attr('data-timerange'))));
    }

    else if($(this).attr('data-date')){
      $(this).html(FnordMetric.util.dateFormat(parseInt($(this).attr('data-date'))));
    }

    else if($(this).attr('data-trend')){
      var delta = parseFloat($(this).attr('data-trend'));
      var deg   = 0;

      if(delta > 0){
        $(this).css('color', '#2B2');  
        $(this).html('+' + delta.toFixed(1));
        deg = -45;
      } else if (delta < 0){
        $(this).css('color', '#B22');  
        $(this).html(delta.toFixed(1));
        deg = 45; 
      } else {
        $(this).css('color', '#555');  
        $(this).html(delta);
      }

      $(this).append(
        $('<i class="icon icon-arrow-right">').css({
          'marginLeft': '0.5em',
          'display': 'block',
          'font-size': '90%',
          'float': 'right',
          '-webkit-transform': 'rotate('+deg+'deg)',
          '-moz-transform': 'rotate('+deg+'deg)'
        })
      )  
    }
   
  });
}

FnordMetric.util.evilFormat = function(str){
  if(str.substring(0, 1) == "$"){
    str = "FnordMetric.util."+str.substring(1)
    return eval(str);
  } else {
    return str;
  }
}