Widgets
-------

FnordMetric includes a collection of UI widgets to display gauge data. You
can use these widgets with a simple ruby DSL.

_Example: Render a timeseries line chart on dashboard "Sales"_

    widget 'Sales',
      :title => "Sales per Minute",
      :gauges => [:sales_per_minute],
      :type => :timeline,
      :width => 100



### TimelineWidget

here be dragons


### NumbersWidget

here be dragons


### BarsWidget

here be dragons


### ToplistWidget

here be dragons


### HTMLWidget

here be dragons


<br />
<br />


Complex Widgets
---------------

There are a few high level widgets which have different semantics. FIXPAUL: explain...


### TimeSeriesGauge

here be dragons
- calculate-option: count, sum, progressive_sum, avg
- incr / incr_counter / incr_denominator


### DistributionGauge

here be dragons


### ToplistGauge

here be dragons


