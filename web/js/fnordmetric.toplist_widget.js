FnordMetric.widgets.toplistWidget = function(){

  var opts;
  var current_gauge = false;

  function render(_opts){
    opts = _opts;

    var headbar = $('<div class="headbar"></div>').append(
      $('<h2></h2>').html(opts.title)
    );

    opts.elem.append(headbar).css({
      'marginBottom': 20,
      'overflow': 'hidden'
    }).append(
      $('<div class="toplist_inner ui_toplist"></div>')
    );

    if(!opts.ticks){
      var first = true;
      for(k in opts.gauges){
        headbar.append(
          $('<div></div>')
            .attr('class', 'button mr')
            .attr('rel', k)
            .append(
              $('<span></span>').html(opts.gauges[k].title)
            ).click(function(){
              loadGauge($(this).attr('rel'));
            }
          )
        );
        if(first){
          first = false;
          loadGauge(k);
        }
      }
    } else {
      loadGauge(opts.gauges[0], true);
    }

    if(opts.autoupdate){
      var secs = parseInt(opts.autoupdate);
      if(secs > 0){

        var autoupdate_interval = window.setInterval(function(){
          loadGauge(false, true);
        }, secs*1000);

        $('body').bind('fm_dashboard_close', function(){
          window.clearInterval(autoupdate_interval);
        });
      }
    };

  }

  function loadGauge(gkey, silent){
    if(!gkey)
      gkey = current_gauge;

    current_gauge = gkey;

    if(!silent)
        $('.toplist_inner', opts.elem).addClass('loading');

    FnordMetric.publish({
      "type": "widget_request",
      "klass": "ToplistWidget",
      "gauge": current_gauge,
      "cmd": "values_for",
      "tick": opts.tick,
      "widget_key": opts.widget_key
    });
  }

  function renderGauge(gkey, gdata){
    var _elem = $('.toplist_inner', opts.elem).removeClass('loading').html('');
    $(gdata.values).each(function(n, _gd){
      var _perc  = (parseInt(gdata.values[n][1]) / parseFloat(gdata.count))*100;
      var _item = $('<div class="toplist_item"><div class="title"></div><div class="value"></div><div class="percent"></div></div>');

      if(opts.click_callback){
        var lelem = $('<a href="#" class="link">').html(gdata.values[n][0]);
        var lclbck; eval("lclbck="+opts.click_callback)
        lelem.attr('data-key', gdata.values[n][0]);
        lelem.click(function(){ lclbck($(this).attr('data-key')); return false; });
        $('.title', _item).html(lelem)
      } else {
        $('.title', _item).html(gdata.values[n][0]);  
      }

      $('.value', _item).html(FnordMetric.util.formatGaugeValue(gkey, parseInt(gdata.values[n][1])));
      $('.percent', _item).html(_perc.toFixed(1) + '%');
      _elem.append(_item);
    });
  }

  function announce(evt){

    if((evt.widget_key == opts.widget_key) && ((evt.class == "widget_response") && (evt.cmd == "values_for")))
        renderGauge(evt.gauge, evt);

  }

  return {
    render: render,
    announce: announce
  };

};
