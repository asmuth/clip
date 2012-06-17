FnordMetric.widgets.toplistWidget = function(){

  var opts;

  function render(_opts){
    opts = _opts;

    var current_gauge = false;

    /* if (!opts.tick){ opts.tick = opts.ticks[0]; } */

    var headbar = $('<div class="headbar"></div>').append(
      $('<h2></h2>').html(opts.title)
    );

    opts.elem.append(headbar).css({
      'marginBottom': 20,
      'overflow': 'hidden'
    }).append(
      $('<div class="toplist_inner"></div>')
    );

    return this;

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

    function loadGauge(gkey, silent){
      if(!gkey){ gkey = current_gauge; }
      current_gauge = gkey;
      if(!silent){ $('.toplist_inner', opts.elem).addClass('loading'); }
      var _url = FnordMetric.p + '/' + FnordMetric.currentNamespace + '/gauge/' + gkey;
      if(opts.tick){ _url += ("?tick=" + opts.tick); }
      $.get(_url, function(_resp){
        var resp = JSON.parse(_resp);
        renderGauge(gkey, resp);
      })
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

  }


  return {
    render: render
  };

};
