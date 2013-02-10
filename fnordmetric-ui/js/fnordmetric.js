var FnordMetric = (function(pre){

  var wsAddress, socket, currentNamespace;
  var widgets = {};

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
    var n, evt = JSON.parse(raw.data);

    if (evt.error)
      return console.log("[FnordMetric] error: " + evt.error);

    if (evt.widget_key && widgets[evt.widget_key])
      widgets[evt.widget_key].send(evt);
  }

  function onSocketOpen() {
    console.log("[FnordMetric] connected...");

    $("*[data-fnordmetric]").each(function(n, e){
      if (!$(e).attr('data-widget-key'))
        refresh(e);
    });
  }

  function onSocketClose() {
    console.log("[FnordMetric] socket closed"); 
    window.setTimeout(connect, 1000);
  }

  return {
    setup: setup,
    publish: publish,
    refresh: refresh,
    resize: resize
  };

})(FnordMetric);
