// FnordMetric UI
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

var FnordMetric = (function(pre){

  var version = "1.2.8";

  var wsAddress, socket, currentNamespace,
     continuations = {},
     continuation = false,
     stack = [],
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

      else if (continuation) {
        continuation(data);
        continuation = false;

        if (stack.length > 0) {
          var nxt = stack.shift();
          execute(nxt[0], nxt[1])
        }
      }

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

  function execute(cmd, cb) {
    if (continuation === false) {
      socket.send(cmd);
      continuation = cb;
    } else {
      stack.push([cmd, cb]);
    }
  }

  function values_in(gauges, since, until, callback) {
    if (enterprise) {
      var all_resp = {};

      function values_in_fetch_next(gauges) {
        var this_resp = gauges.shift();

        if (typeof this_resp == 'undefined')
          return;

        execute(
          "VALUESIN " + this_resp + " " + since + " " + until,
          function(resp) {
            var vals = {},
                parts = resp.split(" ");

            if (parts[0] != "null")
              for (ind in parts) {
                var tuple = parts[ind].split(":");
                tuple[0] = parseInt(parseInt(tuple[0], 10) / 1000, 10);
                vals[tuple[0]] = tuple[1];
              }

            all_resp[this_resp] = vals;

            if (gauges.length == 0)
              callback.apply(FnordMetric.util.zeroFill(all_resp));
            else
              values_in_fetch_next(gauges);
          }
        );
      }

      values_in_fetch_next(gauges);
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
      execute(
        "VALUEAT " + gauge + " " + at,
        function(resp) {
          callback.apply({ "value": eval(resp) });
        }
      );
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
