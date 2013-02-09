if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.util == 'undefined')
  FnordMetric.util = {};

FnordMetric.util.decPrint = function(val){
  return (val < 10 ? '0'+val : val);
}

FnordMetric.util.formatValue = function(value){
  if(value < 10){
    return value.toFixed(1);
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

FnordMetric.util.generateUUID = function (){
  return Math.floor((1 + Math.random()) * 0x1000000).toString(16);
}

FnordMetric.util.updateCounter = function(trgt_elem, diff_factor){
  var still_running = false;
  if(!diff_factor){ diff_factor = 4; }
  $([trgt_elem]).each(function(){
    var target_val = parseFloat($(this).attr('data-value'));
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
      var val_txt = FnordMetric.util.formatValue(new_val);
      if ($(this).attr('data-unit')){ val_txt += $(this).attr('data-unit'); }
      $(this).html(val_txt);
    }
  });
  if(still_running){
    (function(te, df, sg){
      window.setTimeout(function(){ 
        FnordMetric.util.updateCounter(te, df, sg); 
      }, 30);
    })(trgt_elem, diff_factor);
  }
}

