FnordMetric.js_api = (function(){

  var timer = null;
  var update_interval = 1000;

  function init() {
    console.log("yeah");
  }

  function socketOpen(){
    console.log("[FnordMetric] connected...");
    timer = window.setInterval(poll, update_interval);
  }

  function socketClose(){
    console.log("[FnordMetric] socket closed");
    if (timer) window.clearInterval(timer);
    timer = null;
    window.setTimeout(FnordMetric.connect, 1000);
  }

  function socketMessage(raw){
    var evt = JSON.parse(raw.data);

    console.log("[FnordMetric] socket msg");
    console.log(evt);
  }

  function poll() {
    console.log("yeah");
  }

  return {
    init: init,
    socketOpen: socketOpen,
    socketClose: socketClose,
    socketMessage: socketMessage
  };

}());
