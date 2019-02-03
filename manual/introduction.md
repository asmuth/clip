# PlotFX v0.2.0

PlotFX is a tool for creating data-driven graphics; it allows you to create
parametric illustrations such as charts and dashboards using a lightweight
CSS-like syntax.

You can run PlotFX from the command line, where it generates SVG, PNG and PDF
output files. Additionally, you can embed it into your own application using the
C API.

PlotFX is built in the hope that using it will be quick, simple and fun.

**WARNING**: The master branch recently underwent a significant refactoring and the
stability is currently beta/experimental. Maybe not even that. It will take a few
more weeks to stabilize and document everything.

<div class="notice">
  <div style="float:right;"><a class="github-button" data-style="mega" href="https://github.com/plotfx/plotfx" data-count-href="/plotfx/plotfx/stargazers" data-count-api="/repos/plotfx/plotfx#stargazers_count" data-count-aria-label="# stargazers on GitHub" aria-label="Star plotfx/plotfx on GitHub">View on GitHub</a></div>
  Join PlotFX development now on GitHub. We're always appreciating new pull requests, bugs and feature requests!
</div>


## Getting Started

[![A simple line chart](./examples/linecharts/simple_timeseries.svg)](./examples/linecharts/simple_timeseries)

The simple line chart from above was generated with PlotFX using the following
command line:

    $ plotfx --in example_chart.ptx --out example_chart.svg

Here is the input file from which the above plot was generated (`example_chart.ptx`):

    width: 1200px;
    height: 280px;

    scale-y-min: 0;
    scale-y-max: 140;

    lines {
      xs: csv('measurement.csv', time);
      ys: csv('measurement.csv', value2);
      colors: #06c;
    }

    axis {
      position: bottom;
      format: datetime("%H:%M:%S");
    }

    axis {
      position: left;
      layout: linear(20);
    }


More examples can be found on [the examples page](https://github.com/plotfx/plotfx/tree/master/examples).
For a more detailed introduction to PlotFX, see the [Getting Started](/documentation/getting-started) page. 
If you have any questions please don't hesitate to reach out via [the PlotFX email group](http://groups.google.com/group/plotfx).

## More Examples

Here are some example charts that were generated using `plotfx` to give you some
ideas on what is possible. Have a look at the [full list of examples](/examples/)
for more.

<div style="height: .6em;"></div>

_Example: Simple timeseries chart_
<a href="/examples/linecharts/simple_timeseries"><img src="/examples/linecharts/simple_timeseries.svg" style="width: 100%;" /></a>

_Example: Horizontal barchart_
<a href="/examples/barcharts/horizontal_bars"><img src="/examples/barcharts/horizontal_bars.svg" style="width: 100%;" /></a>

_Example: Scatter chart with labels_
<a href="/examples/pointcharts/pointchart_with_labels"><img src="/examples/pointcharts/pointchart_with_labels.svg" style="width: 100%;" /></a>

<script async defer src="https://buttons.github.io/buttons.js"></script>
