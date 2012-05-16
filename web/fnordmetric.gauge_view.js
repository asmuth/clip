FnordMetric.views.gaugeView = (function(gauge_name, conf){
  var widgets = [];
  var widget_objs = {};
  var viewport = null;
  var tabs = [];

  function load(_viewport){
    viewport = _viewport;
    viewport.html('');
    viewport.append('<div class="navbar"></div>');

    viewport.append(
      $('<div class="gauge_viewport"></div>').html(conf.html)
    );

    for(_wkey in conf.widgets){
      if(!conf.widgets[_wkey].tab){
        conf.widgets[_wkey].tab = "Overview"
      }
      if(tabs.indexOf(conf.widgets[_wkey].tab) == -1){ 
        tabs.push(conf.widgets[_wkey].tab)
      }
    }  

    FnordMetric.ui.navbar($('.navbar', viewport), {
      breadcrumb: [ 
        ["Fnord", "/group/fnord"],
        [conf.title, "/gauge/"+gauge_name]
      ],
      buttons: [
        ["Export Data", function(){ alert(23); }]
      ]
    });
  }

  function close(){
    $('body').trigger('fm_dashboard_close');
  }

  function resize(){
    
  }

  return {
    load: load,
    resize: resize,
    close: close
  };

});
