FnordMetric.util.getNextWidgetUID = function(){
  return (FnordMetric.currentWidgetUID += 1);
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

FnordMetric.util.updateNumbers = function(trgt_elem, diff_factor, single){
  var still_running = false;
  if(!diff_factor){ diff_factor = 4; }
  var elems = [];
  if (single) { elems = [trgt_elem]; } else { elems = $('.number', $(trgt_elem)); }
  $(elems).each(function(){
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
      var val_txt = FnordMetric.util.formatGaugeValue($(this).attr('rel'), new_val);
      if ($(this).attr('data-unit')){ val_txt += $(this).attr('data-unit'); }
      if (single) { $(this).html(val_txt); } else { $('.value', this).html(val_txt); }
    }
  });
  if(still_running){
    (function(te, df, sg){
      window.setTimeout(function(){ 
        FnordMetric.util.updateNumbers(te, df, sg); 
      }, 30);
    })(trgt_elem, diff_factor, single);
  }
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
