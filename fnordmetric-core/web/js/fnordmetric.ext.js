var FnordMetric = (function(){

  var require = [
    '/js/d3.fnordmetric.js',
    '/js/rickshaw.fnordmetric.js',
    '/js/fnordmetric.js',
    '/js/fnordmetric.util.js',
    '/js/fnordmetric.timeseries_widget.js',
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
