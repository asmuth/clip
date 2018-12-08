# PlotFX v0.1.0

PlotFX is a command line tool for creating charts from CSV files. It supports a
number of different plot types and other diagrams. Charts can be customized
using a configuration language similar to CSS.

<div class="notice">
  <div style="float:right;"><a class="github-button" data-style="mega" href="https://github.com/plotfx/plotfx" data-count-href="/plotfx/plotfx/stargazers" data-count-api="/repos/plotfx/plotfx#stargazers_count" data-count-aria-label="# stargazers on GitHub" aria-label="Star plotfx/plotfx on GitHub">View on GitHub</a></div>
  Join PlotFX development now on GitHub. We're always appreciating new pull requests, bugs and feature requests!
</div>


## Getting Started

[![A simple line chart](./examples/linecharts/lines_with_points.svg)](./examples/linecharts/lines_with_points.plot)

The simple line chart from above was generated with PlotFX using the following
command line:

    $ plotfx -in example_chart.plot -out example_chart.svg

Running the command from above, will generate the following output file
(`example_chart.svg`):

Input File (`example_chart.plot`):

    plot {
      axis-top: off;
      axis-right: off;

      ymin: -10;
      ymax: 32;

      series {
        title: "New York";
        point-size: 3.3pt;
        xs: csv(testdata/city_temperatures.csv, month);
        ys: csv(testdata/city_temperatures.csv, new_york);
      }

      series {
        title: "北京市";
        point-size: 3.3pt;
        xs: csv(testdata/city_temperatures.csv, month);
        ys: csv(testdata/city_temperatures.csv, beijing);
      }

      # ...
    }


For more examples, please see [the examples page](https://github.com/plotfx/plotfx/tree/master/examples).

To get started with PlotFX, have a look at the [Getting Started](/documentation/getting-started) and
[Data Model](/documentation/data-model) pages. If you have any questions please
don't hesitate to reach out via [Github Issues](http://github.com/plotfx/plotfx/issues)
or [the PlotFX google group](http://groups.google.com/group/fnordmetric).


## More Examples

Here are some example charts that were generated using `plotfx` to give you some
ideas on what is possible. Have a look at the [full list of examples](/examples/)
for more.

<div style="height: .6em;"></div>

_Example: Simple timeseries chart_
<a href="/examples/linecharts/simple_lines"><img src="/examples/areacharts/examples_simple_lines.png" style="width: 100%;" /></a>

_Example: Chart with explicit domain and legend_
<a href="/examples/linecharts/explicit_domain"><img src="/examples/linecharts/examples_explicit_domain.png" style="width: 100%;" /></a>

_Example: Horizontal barchart_
<a href="/examples/barcharts/horizontal_bars"><img src="/examples/barcharts/examples_horizontal_bars.png" style="width: 100%;" /></a>

_Example: Scatter chart with lables_
<a href="/examples/pointcharts/pointchart_with_labels"><img src="/examples/pointcharts/examples_pointchart_with_labels.png" style="width: 100%;" /></a>

<script async defer src="https://buttons.github.io/buttons.js"></script>
