FnordMetric.widgets.realtimeValueWidget = function(){

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var width, height, canvas,  opts, max;

    var xpadding = 20;
    var ypadding = 20;
    var bmargin = 6;
    var bwidth  = 5;
    var bcolor  = '#06C';

    var bars = [];

    var tick = false;

    var next_values = [];
    var next_value_interval = false;

    function render(_opts){
      opts = _opts;

      drawLayout();

      width = opts.elem.width() - (xpadding * 2) - 15 - 250;
      height = opts.height || 240;
      //xtick = width / (xticks - 1);

      canvas = d3.select('#container-'+widget_uid)
        .append("svg:svg")
        .attr("width", width+(2*xpadding))
        .attr("height", height+30);

      canvas.selectAll("*").remove();

      for(var n=parseInt(width / bmargin); n > 0; n--){
        drawValue(false, n-1);
      }

      changeTick(opts.ticks[0]);
    }

    function changeTick(_tick){
      tick = _tick;
      canvas.selectAll('.valuebar').remove();
      next_values = [];
      max = 1;
    }

    function nextValue(value){
      if (value > max){
        var old_max = max;
        max = value * 1.2;
        canvas.selectAll('.valuebar').each(function(){
          var theight = parseInt($(this).attr('height'))*(old_max/max);
          $(this).attr("y", height-(ypadding*2)-theight)
          $(this).attr("height", theight);
        });
      }

      drawValue(value);

      canvas.selectAll('.valuebar').each(function(){
        var new_x = parseInt($(this).attr('x'))-bmargin;
        if(new_x < 5){ new_x = 5; }
        $(this).attr("x",  new_x);
      });

      pruneValues();
      next_value_interval = false;
      nextValueAsync();
    }

    function nextValueAsync(value){
      if(value){ next_values.unshift(value); }
      if(next_values.length > 0){
        if(!next_value_interval){
          next_value_interval = window.setInterval(function(){
            var _v = next_values.pop();
            if(!!_v){ nextValue(_v); }
          }, 20);  
        }
      }   
    }

    function pruneValues(){
      while(bars.length > (width / bmargin)){
        bars.pop().remove();
      }
    }

    function drawValue(value, offset){
      if(!offset){ offset = 0; }
      if(!value){ value = 0; }

      if(value > 0){ 
        var theight = (height-(ypadding*4)) * (value/max);
        var fillc = bcolor;
      } else {
        var theight = 5;
        var fillc = '#ddd';
      }  
      
      bars.unshift(canvas
        .append("svg:rect")
        .attr("class", "valuebar")
        .attr("x", width+xpadding-(offset*bmargin))
        .attr("y", height-(ypadding*2)-theight)
        .attr("fill", fillc)
        .attr("width", bwidth)
        .attr("height", theight));
    }

    function announce(evt){
      if((evt._class == "widget_push") && (evt.cmd == "value")){
        if(evt.values[tick]){
          nextValueAsync(parseFloat(evt.values[tick]));  
        }        
      }
      if(evt.widget_key == opts.widget_key){

      }
    }

    function drawLayout(){
      $(opts.elem).append( $('<div></div>').attr('class', 'headbar').append(
        $('<h2></h2>').html(opts.title)
      ) ).append(
        $('<div></div>').attr('id', 'container-'+widget_uid).css({
          height: opts.height + 6,
          marginBottom: 20,
          overflow: 'hidden'
        })
      );

      if(opts.ticks){
        $('.headbar', opts.elem).append('<div class="tick_btns btn_group" style="margin-right:17px;"></div>');
        for(__tick in opts.ticks){
          var _tick = opts.ticks[__tick];
          $('.tick_btns', opts.elem).append(
            $('<div></div>').attr('class', 'button tick').append($('<span></span>')
              .html(FnordMetric.util.formatTimeRange(_tick)))
              .attr('data-tick', _tick)
              .click(function(){ 
                changeTick($(this).attr('data-tick')); 
              })  
          );
        }
      }
    }

    return {
      render: render,
      announce: announce
    }

};
