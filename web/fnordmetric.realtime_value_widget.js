FnordMetric.widgets.realtimeValueWidget = function(){

    var widget_uid = FnordMetric.util.getNextWidgetUID();
    var width, height, canvas,  opts;

    var xpadding = 20;
    var ypadding = 20;
    var bmargin = 6;
    var bwidth  = 5;

    var bars = [];
    var max = 0;

    function render(_opts){
      opts = _opts;

      drawLayout();

      width = opts.elem.width() - (xpadding * 2) - 15;
      height = opts.height || 240;
      //xtick = width / (xticks - 1);

      canvas = d3.select('#container-'+widget_uid)
        .append("svg:svg")
        .attr("width", width+(2*xpadding))
        .attr("height", height+30);

      canvas.selectAll("*").remove();

      window.setInterval(function(){
        nextValue(Math.random() * 50);  
      }, 60);      

      //canvas.drawGrid(0, 0, width+(2*xpadding), height, 1, 6, "#ececec");
    }

    function nextValue(value){
      if (value > max){
        max = value * 1.2;
      }
      drawValue(value);
      pruneValues();
    }

    function pruneValues(){
      while(bars.length > (width / bmargin)){
        bars.pop().remove();
      }
    }

    function drawValue(value, offset){

      if(!value){ theight = 0; }
      if(!offset){ offset = 0; }

      var theight = (height-(ypadding*4)) * (value/max);

      canvas.selectAll('.valuebar')
        .transition()
        .attr("x", function(){ return parseInt($(this).attr('x'))-bmargin; })
        .duration(20);

      bars.unshift(canvas
        .append("svg:rect")
        .attr("class", "valuebar")
        .attr("x", width+xpadding-(offset*bmargin))
        .attr("y", height-(ypadding*2)-theight)
        .attr("width", bwidth)
        .attr("height", theight));
    }

    function announce(evt){
      if(evt.widget_key == opts.widget_key){
        if((evt._class == "widget_push") && (evt.cmd == "value")){
          console.log('YEAH');
        }
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
        $('.headbar', opts.elem).append('<div class="tick_btns btn_group"></div>');
        for(__tick in opts.ticks){
          var _tick = opts.ticks[__tick];
          $('.tick_btns', opts.elem).append(
            $('<div></div>').attr('class', 'button tick').append($('<span></span>')
              .html(FnordMetric.util.formatTimeRange(_tick)))
              .attr('data-tick', _tick)
              .click(changeTick)  
          );
        }
      }
    }

    return {
      render: render,
      announce: announce
    }

};
