var FnordMetric = (function(){

  var require = [
    '/vendor/d3.v2.js',
    '/vendor/rickshaw.fnordmetric.js',
    '/js/fnordmetric.js',
    '/js/fnordmetric.util.js',
    '/js/fnordmetric.ui.js',
    '/js/fnordmetric.timeline_widget.js',
    '/js/fnordmetric.js_api.js'
  ];

  var setup = function(opts){
    if (typeof $ == 'undefined') {
      console.log("error: FnordMetric requires jQuery 1.6.2+");
      return;
    }

    FnordMetric.currentNamespace = opts.namespace;
    FnordMetric.ws_addr = "ws://" + opts.address + "/stream";

    for (n=0; n < require.length; n++)
      document.write('<script type="text/javascript" src="' + (
        "http://" + opts.address + require[n]) + '"></script>');
  }


  return {
    setup: setup
  };

}());
