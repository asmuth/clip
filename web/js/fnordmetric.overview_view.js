FnordMetric.views.overviewView = function(){
  var widgets = [];
  var viewport = null;

  function load(_viewport){
    viewport = _viewport.html('');

    viewport.append('<div class="navbar"></div>');
    viewport.append('<h1>Application Overview</h1>');
    viewport.append('<h3>Welcome to FnordMetric :)</h3>');

    // $.ajax({
    //   url: FnordMetric.p + '/' + FnordMetric.currentNamespace+'/dashboard/'+dashboard_name,
    //   success: function(resp, status){
    //     var conf = JSON.parse(resp);
    //     renderWidgets(conf.widgets);
    //   }
    // });
  };

  function resize(){
    // here be dragons
  }

  function close(){
    // here be dragons
  }

  return {
    load: load,
    resize: resize,
    close: close
  };

};
