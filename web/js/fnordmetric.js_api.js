FnordMetric.js_api = (function(){

  var timer = null;
  var update_interval = 1000;

  var widgets = [];

  function init() {
    $("div[data-fnordmetric]").each(function(n, e){
      var elem = $(e);

      var widget = FnordMetric.widgets.timeseriesWidget();

      widget.render({
        elem: elem,
        async_chart: true,
        autoupdate: 1,
        default_cardinal: false,
        default_style: "line",
        end_timestamp: 1360281926,
        include_current: true,
        klass: "TimelineWidget",
        gauges: ["user_logins"],
        series: [
          { "color": "#4572a7", "data": [{x:0, y:0}], name: "user_logins" }
        ],
        start_timestamp: 1360263926,
        tick: 60,
        title: "Login and Signup",
        widget_key: "loginandsignup",
        width: 75,
        xticks: 300
      });

      widgets.push(widget);
    });
  }

  function socketOpen(){
    console.log("[FnordMetric] connected...");
    init();
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
    for (n = 0; n < widgets.length; n++)
      widgets[n].announce(evt);
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
