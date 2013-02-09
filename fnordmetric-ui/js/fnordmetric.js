var FnordMetric = (function(){

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

    var widget = FnordMetric.widgets[widget_type]();
    console.log(widget);
  }

  function onSocketMessage(raw) {
    var evt = JSON.parse(raw.data);
    console.log(evt);
  }

  function onSocketOpen() {
    console.log("[FnordMetric] connected...");
  }

  function onSocketClose() {
    console.log("[FnordMetric] socket closed"); 
    window.setTimeout(connect, 1000);
  }

  return {
    setup: setup,
    publish: publish,
    refresh: refresh
  };

})();
