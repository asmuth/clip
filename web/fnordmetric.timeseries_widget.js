FnordMetric.widgets.timeseries_widget = function(){

    /*
      options:
        
        height: 430
        default_cardinal: true/false
        default_style: line/area/stack/flow
        series
    */

    var widget_uid = "fnord-" + parseInt(Math.random()*99990000);
    var width, height, opts, graph, gconfig;

    var cardinal = true;

    //["#FACE4F", "#42436B", "#CD645A", "#2F635E"]

    function render(_opts){
      opts = _opts;

      if(!opts.default_style){ opts.default_style = 'line'; }
      if(!opts.default_cardinal){ opts.default_cardinal = true; }

      draw_layout();

      width = opts.elem.width() - 50;
      height = opts.height || 430;

      gconfig = {
        element: $('.container', opts.elem)[0],
        width: width,
        height: height,
        interpolation: (opts.default_cardinal ? 'cardinal' : 'linear'),
        stroke: true,
        series: opts.series,
        padding: { top: 0.1, bottom: 0.15 }
      }

      graph = new Rickshaw.Graph(gconfig);

      $('.button[data="'+opts.default_style+'"]')
        .addClass('active')
        .trigger('click');

      var legend = new Rickshaw.Graph.Legend({
        graph: graph,
        element: $('.legend', opts.elem)[0]
      });

      hoverDetail = new Rickshaw.Graph.HoverDetail( {
        graph: graph
      });

      shelving = new Rickshaw.Graph.Behavior.Series.Toggle({
        graph: graph,
        legend: legend
      });

      highlighter = new Rickshaw.Graph.Behavior.Series.Highlight({
        graph: graph,
        legend: legend
      });

      new Rickshaw.Graph.Axis.Time({
        graph: graph,
      }).render();

      new Rickshaw.Graph.Axis.Y({
        graph: graph,
      }).render();
    }

    function toggle_cardinal(){
      if($(this).hasClass('active')){ return false; }
      $(this).addClass('active').siblings().removeClass('active');

      if(cardinal === true){
        cardinal = false;
        gconfig.interpolation = 'linear';
      } else {
        cardinal = true;
        gconfig.interpolation = 'cardinal';
      }

      graph.configure(gconfig);
      graph.render();
    }

    function change_style(){
      $(this).addClass('active').siblings().removeClass('active');

      if($(this).attr('data') == 'line'){
        gconfig.renderer = 'line';
        gconfig.offset = 'value';
      }

      if($(this).attr('data') == 'stack'){
        gconfig.renderer = 'area';
        gconfig.offset = 'value';
      }

      if($(this).attr('data') == 'flow'){
        gconfig.renderer = 'stack';
        gconfig.offset = 'wiggle';
      }

      graph.configure(gconfig);
      graph.render();
    }

    function draw_layout(){
      $(opts.elem)
        .append(
          $('<div></div>')
            .addClass('headbar')
            .append($('<h2></h2>').html(opts.title))
            .append(
              $('<div class="btn_group mr"></div>')
              .append(
                $('<div></div>')
                  .addClass('button')
                  .append($('<span>').html('Flow'))
                  .attr('data', 'flow')
                  .click(change_style)
              )
              .append(
                $('<div></div>')
                  .addClass('button')
                  .append($('<span>').html('Stack'))
                  .attr('data', 'stack')
                  .click(change_style)
              )
              .append(
                $('<div></div>')
                  .addClass('button')
                  .append($('<span>').html('Line'))
                  .attr('data', 'line')
                  .click(change_style)
              )
            )
            .append(
              $('<div class="btn_group mr"></div>')
              .append(
                $('<div></div>')
                  .addClass('button mr')
                  .addClass(opts.default_cardinal ? '' : 'active')
                  .append($('<span>').html('Off'))
                  .attr('data', 'cardinal-off')
                  .click(toggle_cardinal)
              )
              .append(
                $('<div></div>')
                  .addClass('button')
                  .addClass(opts.default_cardinal ? 'active' : '')
                  .append($('<span>').html('On'))
                  .attr('data', 'cardinal-on')
                  .click(toggle_cardinal)
              )
            )
        )
        .append(
          $('<div></div>')
            .addClass('legend')
            .css({
              margin: '10px 30px 0 30px',
            })
        )
        .append(
          $('<div></div>')
            .addClass('container')
            .css({
              height: opts.height,
              margin: '0 23px 50px 23px',
            })
        );
    }

    return {
      render: render
    }

};
