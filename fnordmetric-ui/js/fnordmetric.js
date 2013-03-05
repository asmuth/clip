var FnordMetric = (function(pre){

  var wsAddress, socket, currentNamespace,
     continuations = {},
     widgets = {},
     enterprise = false;

  function setup(opts) {
    if (typeof $ == 'undefined') {
      console.log("error: FnordMetric requires jQuery 1.6.2+");
      return;
    }

    currentNamespace = opts.namespace;
    wsAddress = "ws://" + opts.address + "/stream";

    $(document).ready(function(){
      connect();
    });
  }

  function connect() {
    if (enterprise)
      socket = new WebSocket(wsAddress, "fnordmetric_enterprise");
    else
      socket = new WebSocket(wsAddress);

    socket.onmessage = onSocketMessage;
    socket.onclose = onSocketClose;
    socket.onopen = onSocketOpen;
  }

  function publish(obj) {
    if (!obj.namespace)
      obj.namespace = currentNamespace;

    socket.send(JSON.stringify(obj));
  }

  function refresh(elem) {
    var elem = $(elem);

    var widget_key = elem.attr('data-widget-key');
    var widget_type = elem.attr('data-fnordmetric');

    if (!widget_type)
      return console.log("[FnordMetric] element is missing the data-fnordmetric attribute");

    if (widget_key && widgets[widget_key]) {
      widgets[widget_key].destroy();
      delete widgets[widget_key];
    }

    widget_key = FnordMetric.util.generateUUID();
    elem.attr('data-widget-key', widget_key);

    widgets[widget_key] = FnordMetric.widgets[widget_type](elem);
    widgets[widget_key].init();
  }

  function resize() {
    for (widget_key in widgets)
      if (widgets[widget_key].resize)
        widgets[widget_key].resize();
  }

  function onSocketMessage(raw) {
    if (enterprise) {
      var data = raw.data;

      if (data.substr(0,5) == "ERROR")
        return console.log("[FnordMetric] error: " + data.substr(6));

    } else {
      var n, evt = JSON.parse(raw.data);

      if (evt.error)
        return console.log("[FnordMetric] error: " + evt.error);

      if (evt.widget_key && continuations[evt.widget_key])
        continuations[evt.widget_key].apply(evt);

      else if (evt.widget_key && widgets[evt.widget_key])
        widgets[evt.widget_key].send(evt);
    }
  }

  function onSocketOpen() {
    console.log("[FnordMetric] connected...");

    $("*[data-fnordmetric]").each(function(n, e){
      if (!$(e).attr('data-widget-key'))
        refresh(e);
    });
  }

  function onSocketClose(e) {
    if (e.code = 1003 && e.reason == "fnordmetric_enterprise") {
      console.log("[FnordMetric] switching to fnordmetric enterprise protocol")
      enterprise = true;

      $("*[data-fnordmetric]").each(function(n, e){
        $(e).attr('data-widget-key', null)
      });

      window.setTimeout(connect, 10);
    } else {
      console.log("[FnordMetric] socket closed");
      window.setTimeout(connect, 1000);
    }
  }

  function values_in(gauges, since, until, callback) {
    if (enterprise) {
      continuation = function() {
        console.log("FFFUBAR", this);
      }

      socket.send("VALUESIN " + gauges.first + " " + since + " " + until);
    }

    else {
      var txid = FnordMetric.util.generateUUID();

      continuations[txid] = function(){
        var result = {};

        for (ind in this.gauges)
          result[this.gauges[ind].key] = this.gauges[ind].vals;

        callback.apply(result);
      }

      FnordMetric.publish({
        "type": "widget_request",
        "klass": "generic",
        "gauges": gauges,
        "cmd": "values_at",
        "since": since,
        "until": until,
        "widget_key": txid
      });
    }
  }

  function value_at(gauge, at, callback) {
    if (enterprise) {

    }

    else {
      var txid = FnordMetric.util.generateUUID();

      continuations[txid] = function(){
        callback.apply({ "value": this.value });
      }

      FnordMetric.publish({
        "type": "widget_request",
        "klass": "generic",
        "cmd": "values_for",
        "gauge": gauge,
        "at": at,
        "widget_key": txid
      })
    }
  }

  return {
    setup: setup,
    publish: publish,
    refresh: refresh,
    resize: resize,
    value_at: value_at,
    values_in: values_in
  };

})(FnordMetric);
