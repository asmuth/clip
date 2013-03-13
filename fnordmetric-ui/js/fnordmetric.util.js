// FnordMetric UI
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

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

FnordMetric.util.zeroFill = function(obj, since, until) {
  var ticks = []

  for (key in obj) {
    for (tick in obj[key]) {
      var t = parseInt(tick, 10);

      if (ticks.indexOf(t) == -1)
        ticks.push(t);
     }
  }

  if (ticks.length == 0)
    ticks.push(0);

  ticks.sort();

  for (key in obj) {
    var m = /-([0-9]+)$/.exec(key);
    var tl = -1;
    var ts = -1;

    if (m != null)
      ts = parseInt(m[1], 10);

    if (typeof since != "undefined")
      tl = since + ts;

    for (ind in ticks) {
      if (ts > 0 && tl > 0) {
        while (ticks[ind] - tl > ts) {
          tl += ts;
          obj[key][tl] = 0;
        }
      }

      tl = ticks[ind];

      if (typeof obj[key][ticks[ind]] == 'undefined')
        obj[key][ticks[ind]] = 0;
    }

    if (typeof until != "undefined") {
      while (tl < until - ts) {
        tl += ts;
        obj[key][tl] = 0;
      }
    }

  }

  return obj;
}
