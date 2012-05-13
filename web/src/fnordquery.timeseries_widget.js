fnordquery.widgets.timeseries_widget = function(){

    var widget_uid = "fnord-" + parseInt(Math.random()*99990000);
    var width, height, opts;

    function render(_opts){
      opts = _opts;

      draw_layout();

      width = opts.elem.width();
      height = opts.height || 350;

      canvas = d3.select('#container-'+widget_uid)
        .append("svg:svg")
        .attr("width", width)
        .attr("height", height);

      //for (ind in opts.series){

        
        var seriesData = [ [], [], [], [], [], [], [], [], [] ];
        var random = new Rickshaw.Fixtures.RandomData(150);

        for (var i = 0; i < 150; i++) {
          random.addData(seriesData);
        }


        var palette = new Rickshaw.Color.Palette( { scheme: 'classic9' } );

        var graph = new Rickshaw.Graph( {
          element: document.getElementById("chart"),
          width: 1630,
          height: 500,
          renderer: 'area',
          stroke: true,
          series: [
            {
              color: palette.color(),
              data: seriesData[0],
              name: 'Moscow'
            }, {
              color: palette.color(),
              data: seriesData[1],
              name: 'Shanghai'
            }, {
              color: palette.color(),
              data: seriesData[2],
              name: 'Amsterdam'
            }, {
              color: palette.color(),
              data: seriesData[3],
              name: 'Paris'
            }, {
              color: palette.color(),
              data: seriesData[4],
              name: 'Tokyo'
            }, {
              color: palette.color(),
              data: seriesData[5],
              name: 'London'
            }, {
              color: palette.color(),
              data: seriesData[6],
              name: 'New York'
            }
          ]
        } );

        graph.render();

        var hoverDetail = new Rickshaw.Graph.HoverDetail( {
          graph: graph
        } );

        var legend = new Rickshaw.Graph.Legend( {
          graph: graph,
          element: document.getElementById('legend')
        } );

        var shelving = new Rickshaw.Graph.Behavior.Series.Toggle( {
          graph: graph,
          legend: legend
        } );

        var order = new Rickshaw.Graph.Behavior.Series.Order( {
          graph: graph,
          legend: legend
        } );

        var highlighter = new Rickshaw.Graph.Behavior.Series.Highlight( {
          graph: graph,
          legend: legend
        } );

        var smoother = new Rickshaw.Graph.Smoother( {
          graph: graph,
          element: $('#smoother')
        } );

        var ticksTreatment = 'glow';

        var xAxis = new Rickshaw.Graph.Axis.Time( {
          graph: graph,
          ticksTreatment: ticksTreatment
        } );

        xAxis.render();

        var yAxis = new Rickshaw.Graph.Axis.Y( {
          graph: graph,
          tickFormat: Rickshaw.Fixtures.Number.formatKMBT,
          ticksTreatment: ticksTreatment
        } );

        yAxis.render();


    }

    function draw_layout(){
      $(opts.elem)
        .append(
          $('<div></div>')
            .attr('class', 'headbar')
            .append($('<h2></h2>').html(opts.title)) 
        )
        .append(
          $('<div></div>')
            .attr('id', 'container-'+widget_uid).css({
              height: opts.height + 6,
              marginBottom: 20,
              overflow: 'hidden'
            })
        );

      // if(opts.ticks){
      //   $('.headbar', opts.elem).append('<div class="tick_btns btn_group"></div>');
      //   for(__tick in opts.ticks){
      //     var _tick = opts.ticks[__tick];
      //     $('.tick_btns', opts.elem).append(
      //       $('<div></div>').attr('class', 'button tick').append($('<span></span>')
      //         .html(FnordMetric.util.formatTimeRange(_tick)))
      //         .attr('data-tick', _tick)
      //         .click(changeTick)  
      //     );
      //   }
      // }
    }



    return {
      render: render
    }

};
