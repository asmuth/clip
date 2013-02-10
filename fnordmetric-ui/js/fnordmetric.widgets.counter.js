if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.widgets == 'undefined')
  FnordMetric.widgets = {};

FnordMetric.widgets.counter = function(elem){
  var gauge, at, refresh_timer, refresh_interval;
  var widget_key = elem.attr("data-widget-key");

  function init() {
    gauge = elem.attr('data-gauge');

    if (!gauge)
      return console.log("[FnordMetric] element is missing the data-gauge attribute");

    at = elem.attr('data-at');
    if (!at) at = "now";

    requestDataAsync();

    if (refresh_interval = elem.attr('data-autoupdate'))
      refresh_interval = parseFloat(refresh_interval);

    if (refresh_interval)
      refresh_timer = window.setInterval(
        requestDataAsync, refresh_interval * 1000);
  }

  function send(evt) {
    elem.attr('data-value', evt.value);
    updateDisplay();
  }

  function requestDataAsync() {
    FnordMetric.publish({
      "type": "widget_request",
      "klass": "generic",
      "cmd": "values_for",
      "gauge": gauge,
      "at": at,
      "widget_key": widget_key
    })
  }

  function destroy() {
    if (refresh_timer)
      window.clearInterval(refresh_timer);
  }

  function updateDisplay() {
    var target_val = parseFloat(elem.attr('data-value'));
    if(!target_val){ target_val=0; }

    var current_val = parseFloat(elem.attr('data-current'));
    if(!current_val){ current_val=0; }

    var diff = (target_val-current_val) / 4;

    if((diff > 0) && (diff < 1)){ diff=1; }
    if((diff < 0) && (diff > -1)){ diff=-1; }

    if(target_val != current_val){
      var new_val = current_val + diff;

      if((diff > 0) && (new_val > target_val)){ new_val = target_val; }
      if((diff < 0) && (new_val < target_val)){ new_val = target_val; }

      var val_txt = FnordMetric.util.formatValue(new_val);

      if (elem.attr('data-unit'))
        val_txt += elem.attr('data-unit');

      elem.attr('data-current', new_val);
      elem.html(val_txt);

      (function(){
        window.setTimeout(updateDisplay, 30);
      })();
    }
  }

  return {
    init: init,
    send: send,
    destroy: destroy
  };

};
