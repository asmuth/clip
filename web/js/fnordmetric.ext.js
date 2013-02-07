var FnordMetric = (function(){

  var require = [
    '/vendor/d3.v2.js',
    '/vendor/rickshaw.fnordmetric.js',
    '/js/fnordmetric.js',
    '/js/fnordmetric.util.js',
    '/js/fnordmetric.ui.js',
    '/js/fnordmetric.timeline_widget.js'
  ];

  var setup = function(opts){
    if (typeof $ == 'undefined') {
      console.log("error: FnordMetric requires jQuery 1.6.2+");
      return;
    }

    this.ext_opts = opts;

    for (n=0; n < require.length; n++)
      document.write('<script type="text/javascript" src="' + (
        "http://" + opts.address + require[n]) + '"></script>');
  }


  return {
    ext_opts: null,
    setup: setup
  };

}());



document.write()
