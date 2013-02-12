Widgets
-------

FnordMetric includes a collection of UI widgets to display the data /
content of gauges. There is a simple ruby DSL to use these widgets.

_Example: Render a timeseries line chart on dashboard "Sales"_

    widget 'Sales',
      :title => "Sales per Minute",
      :gauges => [:sales_per_minute],
      :type => :timeline,
      :width => 100



### TimelineWidget

<img src="/img/widget_timeseries.png" width="600" />


### NumbersWidget

<img src="/img/widget_numbers.png" width="500" />


### BarsWidget

<img src="/img/widget_bars.png" width="600" />


### ToplistWidget

<img src="/img/widget_toplist.png" width="500" />


### HTMLWidget

here be dragons


