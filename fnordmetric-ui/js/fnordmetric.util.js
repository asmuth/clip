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

FnordMetric.util.dateFormat = function(timestamp){
  var t = new Date(timestamp*1000);
  return FnordMetric.util.decPrint(t.getDate())      + "." + 
         FnordMetric.util.decPrint((t.getMonth()+1)) + "." + 
         FnordMetric.util.decPrint(t.getFullYear())  + " " +
         FnordMetric.util.decPrint(t.getHours())     + ":" +
         FnordMetric.util.decPrint(t.getMinutes())   + " ";
}

FnordMetric.util.generateUUID = function (){
  return Math.floor((1 + Math.random()) * 0x1000000).toString(16);
}
