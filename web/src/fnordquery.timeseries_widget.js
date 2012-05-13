fnordquery.widgets.timeseries_widget = function(){

    var widget_uid = "fnord-" + parseInt(Math.random()*99990000);
    var width, height, opts;

    function render(_opts){
      opts = _opts;

      draw_layout();

      width = opts.elem.width() - 65;
      height = opts.height || 400;

      //for (ind in opts.series){


        var graph = new Rickshaw.Graph( {
          element: $('.container', opts.elem)[0],
          width: width,
          height: height,
          renderer: 'stack',
          offset: 'wiggle',
          interpolation: 'cardinal',
          stroke: true,
          series: opts.series,
        } );

        graph.render();

        var hoverDetail = new Rickshaw.Graph.HoverDetail( {
          graph: graph
        });

        var legend = new Rickshaw.Graph.Legend( {
          graph: graph,
          element: document.getElementById('legend')
        });

        var shelving = new Rickshaw.Graph.Behavior.Series.Toggle( {
          graph: graph,
          legend: legend
        });

        var order = new Rickshaw.Graph.Behavior.Series.Order( {
          graph: graph,
          legend: legend
        });

        var highlighter = new Rickshaw.Graph.Behavior.Series.Highlight( {
          graph: graph,
          legend: legend
        });

        var smoother = new Rickshaw.Graph.Smoother( {
          graph: graph,
          element: $('#smoother')
        });

        var xAxis = new Rickshaw.Graph.Axis.Time( {
          graph: graph,
          ticksTreatment: 'glow'
        });

        xAxis.render();

        var yAxis = new Rickshaw.Graph.Axis.Y( {
          graph: graph,
          tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
          ticksTreatment: 'glow'
        } );

        yAxis.render();

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
                  .attr('data', 'line')
                  .click(change_style)
              )
              .append(
                $('<div></div>')
                  .addClass('button')
                  .append($('<span>').html('Stack'))
                  .attr('data', 'line')
                  .click(change_style)
              )
              .append(
                $('<div></div>')
                  .addClass('button')
                  .append($('<span>').html('Area'))
                  .attr('data', 'line')
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
                  .append($('<span>').html('Off'))
                  .attr('data', 'line')
                  .click(change_style)
              )
              .append(
                $('<div></div>')
                  .addClass('button')
                  .append($('<span>').html('On'))
                  .attr('data', 'line')
                  .click(change_style)
              )
            )
        )
        .append(
          $('<div></div>')
            .addClass('container')
            .css({
              height: opts.height,
              margin: '50px 30px 40px 30px',
            })
        );
        

      // if(opts.ticks){
      //   $('.headbar', opts.elem);
      //   for(__tick in opts.ticks){
      //     var _tick = opts.ticks[__tick];
      //     $('.tick_btns', opts.elem)
      //   }
      // }
    }

    function change_style(){}



    return {
      render: render
    }

};
