fnordquery.views.report = (function(report_token){
  var opts;
  var viewport = null;

  function load(_viewport){
    viewport = _viewport;
    viewport.html('');
    viewport.append('<div class="navbar"></div>');

    $.ajax({
      type: 'get',
      url: '/report/' + report_token + '.json',
      success: function(raw){
        resp = JSON.parse(raw);
        finally_load(resp);
      }
    })
  };

  function finally_load(_opts){
    opts = _opts;

    fnordquery.ui.navbar($('.navbar', viewport), {
      breadcrumb: [ 
        ["Fnord", "/group/fnord"],
        [opts.title, "/report/"+report_token]
      ],
      buttons: [
        ["Export Data", function(){ alert(23); }]
      ]

    });

    fnordquery.ui.modal({
      height: 700
    })
  }


  function resizeWidget(wkey){
    // var widget = widgets[wkey];
    // var wwperc = widgets[wkey].width;
    // if(!wwperc){ wwperc = 100; }
    // var wwidth = viewport.width() * (wwperc/100.0);
    // if(wwperc==100){
    //   widgets[wkey].elem.addClass('full_width');
    // } else { wwidth -= 1; }
    // widget.elem.width(wwidth);
  }

  function resize(){
    
  }

  function close(){
    //$('body').trigger('fm_dashboard_close');
  };

  return {
    load: load,
    resize: resize,
    close: close
  };

});
