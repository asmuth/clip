
var FnordMetricMetricSeriesListChart = function(elem) {
  'use strict';

  this.render = function(series) {
    for (var i = 0; i < series.length; i++) {
      if (series[i].tags && series[i].tags.indexOf("summary") > -1) {
        renderChart(series[i].time, series[i].values);
        renderSummaries(series[i].summaries);
        break;
      }
    }
  }

  var renderChart = function(time, values) {
  
  }

  var renderSummaries = function(summaries) {
    console.log(elem, summaries);
  }
}
