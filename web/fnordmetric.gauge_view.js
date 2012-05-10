FnordMetric.views.gaugeView = (function(gauge_name, conf){
  var widgets = [];
  var widget_objs = {};
  var viewport = null;

  function load(_viewport){
    viewport = _viewport;
    viewport.html('');
    viewport.append('<div class="navbar"></div>');
    viewport.append('<h1>' + gauge_name + '</h1>');
    viewport.append('<h3>Loading...</h3>');
    viewport.append('<div class="gauge_viewport loading"></div>');

    $('h1', viewport).html(conf.title);
    $('h3', viewport).html(conf.title);
    $('.gauge_viewport', viewport).html(conf.template);
    $('.gauge_viewport', viewport).removeClass('loading');

    FnordMetric.ui.navbar($('.navbar', viewport), {
      breadcrumb: [ 
        ["Fnord", "/group/fnord"],
        [conf.title, "/gauge/"+gauge_name]
      ],
      buttons: [
        ["Export Data", function(){ alert(23); }]
      ]
    });

    renderWidgets(conf.widgets);
  };

  function announce(evt){
    if(evt._class == "widget_response"){
      for(_wkey in widgets){
        widget_objs[_wkey].announce(evt);
      }  
    }
  }

  function renderWidgets(_widgets){
    for(wkey in _widgets){
      var widget = _widgets[wkey];
      //widget["elem"] = $(widget["render_target"], viewport);
      widget["elem"] = $('<div class="widget"></div>');
      $('.viewport_inner').append(widget["elem"]);
      widgets[wkey] = widget;
      resizeWidget(wkey);
      renderWidget(wkey);
    };
    resize();
  };

  function renderWidget(wkey, _w){
    var widget = widgets[wkey];
    /* argh... */
    if(widget.klass=='TimelineWidget'){ _w = FnordMetric.widgets._timelineWidget(); }
    if(widget.klass=='BarsWidget'){ _w = FnordMetric.widgets.barsWidget(); }
    if(widget.klass=='NumbersWidget'){ _w = FnordMetric.widgets.numbersWidget(); }
    if(widget.klass=='ToplistWidget'){ _w = FnordMetric.widgets.toplistWidget(); }
    if(widget.klass=='PieWidget'){ _w = FnordMetric.widgets.pieWidget(); }
    if(widget.klass=="HtmlWidget") { _w = FnordMetric.widgets.htmlWidget(); }
    if(_w){ _w.render(widget); }
    widget_objs[wkey] = _w;
  };

  function resizeWidget(wkey){
    var widget = widgets[wkey];
    var wwperc = widgets[wkey].width;
    if(!wwperc){ wwperc = 100; }
    var wwidth = viewport.width() * (wwperc/100.0);
    if(wwperc==100){
      widgets[wkey].elem.addClass('full_width');
    } else { wwidth -= 1; }
    widget.elem.width(wwidth);
  }

  function resize(){
    for(wkey in widgets){
      resizeWidget(wkey);
    };
  };

  function close(){
    $('body').trigger('fm_dashboard_close');
  };

  return {
    load: load,
    resize: resize,
    announce: announce,
    close: close
  };

});
