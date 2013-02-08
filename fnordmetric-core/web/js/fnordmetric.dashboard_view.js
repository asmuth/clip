FnordMetric.views.dashboardView = (function(dashboard_name){
    var widgets = [];
    var widget_objs = {};
    var viewport = null;

    function load(_viewport){
      viewport = _viewport.html('');
      viewport.append('<div class="navbar"></div>');
      viewport.append('<div class="dashboard_view"></div>');

      FnordMetric.ui.navbar($('.navbar', viewport), {
        breadcrumb: [ 
          ["Dashboard", "/group/fnord"],
          [dashboard_name, "/dashboard/"+dashboard_name]
        ],
        buttons: [
          ["<i class='icon-refresh'></i>Refresh", function(){ load(viewport); }]
        ]
      });

      $.ajax({
        url: FnordMetric.p + '/' + FnordMetric.currentNamespace+'/dashboard/'+dashboard_name,
        success: function(resp, status){
          var conf = JSON.parse(resp);
          renderWidgets(conf.widgets);
        }
      });
    };

    function renderWidgets(_widgets){
      for(wkey in _widgets){
        var widget = _widgets[wkey];
        widget["elem"] = $('<div class="widget"></div>');
        widgets[wkey] = widget;
        $('.dashboard_view', viewport).append(widget.elem);
        resizeWidget(wkey);
        renderWidget(wkey);
      };
      resize();
    };

    function renderWidget(wkey, _w){
      var widget = widgets[wkey];
      if(!widget["widget_key"]){ widget["widget_key"] = wkey; }
      /* argh... */
      if(widget.klass=='TimelineWidget'){ _w = FnordMetric.widgets.timeseriesWidget(); }
      if(widget.klass=='NumbersWidget'){ _w = FnordMetric.widgets.numbersWidget(); }

      if(widget.klass=='ToplistWidget'){ _w = FnordMetric.widgets.toplistWidget(); }

      if(widget.klass=='BarsWidget'){ _w = FnordMetric.widgets.barsWidget(); }
      if(widget.klass=='PieWidget'){ _w = FnordMetric.widgets.pieWidget(); }
      if(widget.klass=="HtmlWidget") { _w = FnordMetric.widgets.htmlWidget(); }

      if(_w){ 
        _w.render(widget);
        widget_objs[wkey] = _w;
      }
    }

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

    function announce(evt){
      if((evt.class == "widget_response") || (evt.class == "widget_push")){
        for(wkey in widget_objs){
          if(widget_objs[wkey].announce){ 
            widget_objs[wkey].announce(evt)
          }
        }  
      }
    }

    return {
      load: load,
      resize: resize,
      announce: announce,
      close: close
    };

  });
