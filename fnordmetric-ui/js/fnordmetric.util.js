if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.util == 'undefined')
  FnordMetric.util = {};

FnordMetric.util.decPrint = function(val){
  return (val < 10 ? '0'+val : val);
}

FnordMetric.util.formatValue = function(value, round_to){
  if(value < 10){
    if (typeof round_to != 'undefined')
      return value.toFixed(round_to);
    else
      return value.toFixed(1);
  }
  if(value < 100){
    if (typeof round_to != 'undefined')
      return value.toFixed(round_to);
    else
      return value.toFixed(1);
  } else if(value > 1000){
    if (typeof round_to != 'undefined')
      return (value/1000.0).toFixed(round_to) + "k";
    else
      return (value/1000.0).toFixed(1) + "k";
  } else {
    if (typeof round_to != 'undefined')
      return value.toFixed(round_to);
    else
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
  return FnordMetric.util.decPrint(t.getFullYear())  + "/" +
         FnordMetric.util.decPrint((t.getMonth()+1)) + "/" + 
         FnordMetric.util.decPrint(t.getDate())      + " " + 
         FnordMetric.util.decPrint(t.getHours())     + ":" +
         FnordMetric.util.decPrint(t.getMinutes())   + " ";
}

FnordMetric.util.dateFormatWithRange = function(timestamp, range){
  var t = new Date(timestamp*1000);

  if (range >= (3600 * 24))
    return FnordMetric.util.decPrint((t.getMonth()+1)) + "/" + 
           FnordMetric.util.decPrint(t.getDate())      + " " + 
           FnordMetric.util.decPrint(t.getHours())     + ":" +
           FnordMetric.util.decPrint(t.getMinutes());
  else if (range >= 900)
    return FnordMetric.util.decPrint(t.getHours())     + ":" +
           FnordMetric.util.decPrint(t.getMinutes());
  else
    return FnordMetric.util.decPrint(t.getHours())     + ":" +
           FnordMetric.util.decPrint(t.getMinutes())   + ":" +
           FnordMetric.util.decPrint(t.getSeconds());
}

FnordMetric.util.generateUUID = function () {
  return Math.floor((1 + Math.random()) * 0x1000000).toString(16);
}

FnordMetric.util.parseTime = function(str) {
  var res,
      now = parseInt(((new Date()).getTime() / 1000), 10),
      str = str.toLowerCase();

  if (str == "now") {
    return now;
  } else if ((res = str.match( /^([0-9]+)$/)) != null) {
    return parseInt(res[1], 10);
  } else if ((res = str.match( /^-([0-9]+)$/)) != null) {
    return (now - parseInt(res[1], 10));
  } else if ((res = str.match( /^-([0-9]+)s(ec(ond)?(s?))?$/)) != null) {
    return (now - parseInt(res[1], 10));
  } else if ((res = str.match( /^-([0-9]+(?:\.[0-9]+)?)m(in(ute)?(s?))?$/)) != null) {
    return parseInt(now - (parseFloat(res[1]) * 60), 10);
  } else if ((res = str.match( /^-([0-9]+(?:\.[0-9]+)?)h(our(s?))?$/)) != null) {
    return parseInt(now - (parseFloat(res[1]) * 3600), 10);
  } else if ((res = str.match( /^-([0-9]+(?:\.[0-9]+)?)d(ay(s?))?$/)) != null) {
    return parseInt(now - (parseFloat(res[1]) * 86400), 10);
  } else {
    console.log("[FnordMetric] invalid time specifiation: " + str);
  }
}

FnordMetric.util.zeroFill = function(obj) {
  var ticks = {};

  for (key in obj)
    for (tick in obj[key])
      ticks[tick] = 1;

  ticks = Object.keys(ticks);

  if (ticks.length == 0)
    ticks.push(0);

  for (key in obj)
    for (ind in ticks)
      if (typeof obj[key][ticks[ind]] == 'undefined')
        obj[key][ticks[ind]] = 0;

  return obj;
}
