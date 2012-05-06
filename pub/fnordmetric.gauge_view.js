FnordMetric.views.gaugeView = (function(gauge_name){
  var widgets = [];
  var viewport = null;

  function load(_viewport){
    viewport = _viewport;
    viewport.html('');
    viewport.append('<div class="navbar"></div>');
    viewport.append('<h1>' + gauge_name + '</h1>');
    viewport.append('<h3>Loading...</h3>');
  };




  function resize(){

  };

  function close(){

  };

  return {
    load: load,
    resize: resize,
    close: close
  };

});