var FnordMetric = (function(){

  var require = [
    '/vendor/d3.v2.js',
    '/vendor/rickshaw.fnordmetric.js',
    '/js/fnordmetric.js',
    '/js/fnordmetric.util.js',
    '/js/fnordmetric.ui.js',
    '/js/fnordmetric.timeseries_widget.js',
    '/js/fnordmetric.js_api.js'
  ];

  var require_css = [
    '/vendor/rickshaw.css',
    '/fnordmetric.rickshaw.css'
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

    for (n=0; n < require_css.length; n++)
      document.write('<link type="text/css" rel="stylesheet" href="' + (
        "http://" + opts.address + require_css[n]) + '" />');
  }


  return {
    setup: setup
  };

}());
