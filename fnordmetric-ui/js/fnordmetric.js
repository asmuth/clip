var FnordMetric = (function(){

  var ws_addr, socket, current_namespace;

  function setup(opts) {
    if (typeof $ == 'undefined') {
      console.log("error: FnordMetric requires jQuery 1.6.2+");
      return;
    }

    current_namespace = opts.namespace;
    ws_addr = "ws://" + opts.address + "/stream";

    $(document).ready(function(){
      connect();
    });
  }

  function connect() {
    socket = new WebSocket(ws_addr);
    socket.onmessage = on_socket_message;
    socket.onclose = on_socket_close;
    socket.onopen = on_socket_open;
  }

  function publish(obj) {
    if (!obj.namespace)
      obj.namespace = current_namespace;

    socket.send(JSON.stringify(obj));
  }

  function refresh(elem) {
    
  }

  function on_socket_message(raw) {
    var evt = JSON.parse(raw.data);
    console.log(evt);
  }

  function on_socket_open() {
    console.log("[FnordMetric] connected...");
  }

  function on_socket_close() {
    console.log("[FnordMetric] socket closed"); 
    window.setTimeout(connect, 1000);
  }

  return {
    setup: setup,
    publish: publish,
    refresh: refresh
  };

})();
