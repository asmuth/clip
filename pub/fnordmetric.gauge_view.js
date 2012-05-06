FnordMetric.views.gaugeView = (function(gauge_name){
  var widgets = [];
  var viewport = null;

  function load(_viewport){
    viewport = _viewport;
    viewport.html('');
    viewport.append('<div class="navbar"></div>');
    viewport.append('<h1>' + gauge_name + '</h1>');
    viewport.append('<h3>Loading...</h3>');
    viewport.append('<div class="gauge_viewport loading"></div>');

    $.ajax({
      url: FnordMetric.p + '/' + FnordMetric.currentNamespace+'/mgauge/'+gauge_name,
      success: function(resp, status){
        var conf = JSON.parse(resp);
        $('h1', viewport).html(conf.title);
        $('h3', viewport).html(conf.title);
        $('.gauge_viewport', viewport).html(conf.template);
        $('.gauge_viewport', viewport).removeClass('loading');
        renderWidgets(conf.widgets);
      }
    });
  };


  function renderWidgets(_widgets){
    for(wkey in _widgets){
      var widget = _widgets[wkey];
      widget["elem"] = $(widget["render_target"], viewport);
      widgets[wkey] = widget;
      resizeWidget(wkey);
      renderWidget(wkey);
    };
    resize();
  };

  function renderWidget(wkey){
    var widget = widgets[wkey];
    /* argh... */
    if(widget.klass=='TimelineWidget'){ FnordMetric.widgets.timelineWidget().render(widget); }
    if(widget.klass=='BarsWidget'){ FnordMetric.widgets.barsWidget().render(widget); }
    if(widget.klass=='NumbersWidget'){ FnordMetric.widgets.numbersWidget().render(widget); }
    if(widget.klass=='ToplistWidget'){ FnordMetric.widgets.toplistWidget().render(widget); }
    if(widget.klass=='PieWidget'){ FnordMetric.widgets.pieWidget().render(widget); }
    if(widget.klass=="HtmlWidget") { FnordMetric.widgets.htmlWidget().render(widget); }
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
    close: close
  };

});
