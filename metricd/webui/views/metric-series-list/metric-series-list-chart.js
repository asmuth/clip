
/**
  {
    summary: {
      series_id: "cur_value"
    }
    time: [1...., 1....,]
    series: [
      {
        series_id: "cur_value",
        values: [2, 6, 7, ...],
        color: "#999",
        title: "Current Value"
        unit: "%"
      }
    ]
  }


**/

var FnordMetricMetricSeriesListChart = function(elem, config) {
  'use strict';

  var default_colors = ["#19A2E5", "#aad4e9"];

  var summary_html = "<div class='total'>{{sum}} {{unit}}</div>" +
    "<div class='legend'>{{legend}}</div>" +
    "<div class='stats'>min={{min}} max={{max}} stddev={{stddev}}</div>";

  var legend_item_html = "<div class='legend_item'>" +
    "<span class='circle' style='background: {{color}}'></span>" +
    "<span>{{title}}</span></div>";

  function render() {
    if (config.hasOwnProperty("summary") &&
        config.summary.hasOwnProperty("series_id")) {
      renderSummaries();
    }
  }

  function renderSummaries() {
    var summary_elem = document.createElement("div");
    summary_elem.className = "summary";

    var html = summary_html;
    var legend_html = [];

    for (var i = 0; i < config.series.length; i++) {
      var series = config.series[i];

      if (series.series_id == config.summary.series_id) {
        /** add sum and stats values **/
        if (series.hasOwnProperty("summaries")) {
          series.summaries.forEach(function(summary) {
            switch (summary.summary) {
              case "sum":
                html = html.replace("{{sum}}", summary.value);
                break;

              case "min":
                html = html.replace("{{min}}", summary.value);
                break;

              case "max":
                html = html.replace("{{max}}", summary.value);
                break;

              case "stddev":
                html = html.replace("{{stddev}}", summary.value);
                break;

              default:
                break;
            }
          });
        }

        /** add unit **/
        if (series.hasOwnProperty("unit")) {
          html = html.replace("{{unit}}", series.unit);
        }
      }

      /** render legend item **/
      var item_html = legend_item_html;
      item_html = item_html.replace(
          "{{color}}",
          series.color ? series.color : default_colors[i]);
      item_html = item_html.replace(
          "{{title}}",
          series.title ? series.title : series.series_id);

      legend_html.push(item_html);
    }

    html = html.replace("{{legend}}", legend_html.join(""));

    html = html.replace(new RegExp('{{[a-z]*}}', 'g'), "&mdash;");
    summary_elem.innerHTML = html;
    elem.appendChild(summary_elem);
  }

 // this.render = function(series) {
 //   for (var i = 0; i < series.length; i++) {
 //     if (series[i].tags && series[i].tags.indexOf("summary") > -1) {
 //       renderChart(series[i].time, series[i].values);
 //       renderSummaries(series[i].summaries);
 //       break;
 //     }
 //   }
 // }

 // var renderChart = function(time, values) {
 // 
 // }

 // var renderSummaries = function(summaries) {
 //   var stats_elem = elem.querySelector(".stats");
 //   stats_elem.innerHTML = [
 //     
 //   ];
 // }

  /** init **/
  render();
}
