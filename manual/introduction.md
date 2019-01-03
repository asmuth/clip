# PlotFX v0.1.0

PlotFX is a standalone tool for creating charts and other data-driven graphics.

Charts are defined using a lightweight syntax that is very similar to CSS.
You can run PlotFX from the command line, where it generates SVG, PNG and PDF
output files. Additionally, you can embed it into your own application using the
C API.

PlotFX is partially based on ideas from the "Grammar of Graphics" [0].
One of the main goals for PlotFX is that creating charts should be quick, simple
and fun.

**WARNING**: The project is currently a work-in-progress. I'm currently working
on removing the old SQL chart specification language and replacing it with the
new CSS-like syntax. This might take a while...

<div class="notice">
  <div style="float:right;"><a class="github-button" data-style="mega" href="https://github.com/plotfx/plotfx" data-count-href="/plotfx/plotfx/stargazers" data-count-api="/repos/plotfx/plotfx#stargazers_count" data-count-aria-label="# stargazers on GitHub" aria-label="Star plotfx/plotfx on GitHub">View on GitHub</a></div>
  Join PlotFX development now on GitHub. We're always appreciating new pull requests, bugs and feature requests!
</div>


## Getting Started

[![A simple line chart](./examples/linecharts/lines_with_points.svg)](./examples/linecharts/lines_with_points)

The simple line chart from above was generated with PlotFX using the following
command line:

    $ plotfx --in example_chart.ptx --out example_chart.svg

Here is the input file from which the above plot was generated (`example_chart.ptx`):

    width: 1200px;
    height: 480px;

    plot {
      data: csv('tests/testdata/city_temperatures.csv');
      x: month;
      y: temperature;
      group: city;

      axis-y-min: -10;
      axis-y-max: 32;

      layer {
        type: lines;
        stroke: 2pt;
      }

      legend {
        position: top left inside;
      }
    }


More examples can be found on [the examples page](https://github.com/plotfx/plotfx/tree/master/examples).
For a more detailed introduction to PlotFX, see the [Getting Started](/documentation/getting-started) page. 
If you have any questions please don't hesitate to reach out via [the PlotFX email group](http://groups.google.com/group/fnordmetric).

## More Examples

Here are some example charts that were generated using `plotfx` to give you some
ideas on what is possible. Have a look at the [full list of examples](/examples/)
for more.

<div style="height: .6em;"></div>

_Example: Simple timeseries chart_
<a href="/examples/linecharts/simple_timeseries"><img src="/examples/linecharts/simple_timeseries.svg" style="width: 100%;" /></a>

_Example: Chart with explicit domain and legend_
<a href="/examples/linecharts/explicit_domain"><img src="/examples/linecharts/explicit_domain.svg" style="width: 100%;" /></a>

_Example: Horizontal barchart_
<a href="/examples/barcharts/horizontal_bars"><img src="/examples/barcharts/horizontal_bars.svg" style="width: 100%;" /></a>

_Example: Scatter chart with lables_
<a href="/examples/pointcharts/pointchart_with_labels"><img src="/examples/pointcharts/pointchart_with_labels.svg" style="width: 100%;" /></a>

<script async defer src="https://buttons.github.io/buttons.js"></script>
