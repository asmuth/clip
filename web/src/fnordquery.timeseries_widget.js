fnordquery.widgets.timeseries_widget = function(){

    var widget_uid = "fnord-" + parseInt(Math.random()*99990000);
    var width, height, opts, graph;

    //["#FACE4F", "#42436B", "#CD645A", "#2F635E"]

    function render(_opts){
      opts = _opts;

      draw_layout();

      width = opts.elem.width() - 50;
      height = opts.height || 430;

      graph = new Rickshaw.Graph( {
        element: $('.container', opts.elem)[0],
        width: width,
        height: height,
        renderer: 'stack',
        offset: 'wiggle',
        interpolation: 'cardinal',
        stroke: true,
        series: opts.series,
        padding: {
          top: 0.1,
          bottom: 0.15
        }
      });

      graph.render();

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

      new Rickshaw.Graph.Axis.Time( {
        graph: graph,
        ticksTreatment: 'glow'
      }).render();

      new Rickshaw.Graph.Axis.Y( {
        graph: graph,
        tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
        ticksTreatment: 'glow'
      }).render();
    }

    function change_style(){}

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
