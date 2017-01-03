
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

  var summary_elem_width = config.hasOwnProperty("summary") ? 225 : 0;
  var grid_margin_x = 20;
  var grid_margin_y = 10;
  var tick_margin = 6;

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

    renderChart();
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

  function renderChart() {
    var chart_elem = document.createElement("div");
    chart_elem.className = "chart";


    var height = elem.offsetHeight;
    var width = elem.offsetWidth - summary_elem_width;

    //FIXME fix viewBox values
    var html = ["<svg class='fm-chart' viewBox='0 0 ", width, " ", height, "'>"];

    html.push(renderXAxis(chart_elem, height, width));
    html.push(renderYAxis(chart_elem, height, width));

    html.push("</svg>");

    chart_elem.innerHTML = html.join("");
    elem.appendChild(chart_elem);
  }

  function renderXAxis(chart_elem, height, width) {
    var tick_height = height - tick_margin;
    var grid_height = height - grid_margin_x;
    var grid_width = width - grid_margin_y;
    var html = [];

    html.push("<g class='axis x'>");

    /** render x ticks **/
    //FIXME make 70 constant
    html.push("<text x='", 0 + 60, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:32", "</text>");

    html.push("<text x='", width * 1/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:32", "</text>");

    html.push("<text x='", width * 2/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 3/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 4/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 5/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 6/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    html.push("<text x='", width * 7/8 + 70, "' y='", tick_height,
      "' class='label'>", "2017-01-02 15:30", "</text>");

    /** render y ticks **/
    html.push("<text x='", grid_margin_y, "' y='", grid_height * 2 / 3,
      "' class='label'>", "10", "</text>");

    html.push("<text x='", grid_margin_y, "' y='", grid_height * 1 / 3,
      "' class='label'>", "10", "</text>");

    html.push("<text x='", grid_margin_y, "' y='", 0,
      "' class='label'>", "10", "</text>");


    /** render x axes **/
    html.push(
      "<line class='axis stroke main_axis' y1='", grid_height, "' y2='",
      grid_height, "' x1='0' x2='", grid_width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='", grid_height * 2 / 3, "' y2='",
      grid_height * 2 / 3, "' x1='", grid_margin_y,
      "' x2='", grid_width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='", grid_height * 1 / 3, "' y2='",
      grid_height * 1 / 3, "' x1='", grid_margin_y,
      "' x2='", grid_width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='0'",
      " x1='", grid_margin_y, "' x2='", grid_width, "'></line>");

    html.push("</g>");
    return html.join("");
  }

  function renderYAxis(chart_elem, height, width) {
    var grid_height = height - grid_margin_x;
    var grid_width = width - grid_margin_y;
    var html = [];

    html.push("<g class='axis y'>");

    /** render y axes **/
    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 1 / 5, "' x2='", grid_width * 1 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 2 / 5, "' x2='", grid_width * 2 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 3 / 5, "' x2='", grid_width * 3 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", grid_height,
      "' x1='", grid_width * 4 / 5, "' x2='", grid_width * 4 / 5, "'></line>");

    html.push("</g>");
    return html.join("");
  }

  /** init **/
  render();
}
