
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

    html.push(renderGrid(chart_elem, height, width));

    html.push("</svg>");

    chart_elem.innerHTML = html.join("");
    elem.appendChild(chart_elem);

  }

  //FIXME better naming
  function renderGrid(chart_elem, height, width) {
    var html = [];

    html.push("<g class='axis'>");

    /** render x axes **/
    html.push(
      "<line class='axis stroke main_axis' y1='", height, "' y2='", height,
      "' x1='0' x2='", width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='", height * 2 / 3, "' y2='", height * 2 / 3,
      "' x1='0' x2='", width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='", height * 1 / 3, "' y2='", height * 1 / 3,
      "' x1='0' x2='", width, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='0'",
      " x1='0' x2='", width, "'></line>");

    /** render y axes **/
    html.push(
      "<line class='axis stroke' y1='0' y2='", height,
      "' x1='", width * 1 / 5, "' x2='", width * 1 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", height,
      "' x1='", width * 2 / 5, "' x2='", width * 2 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", height,
      "' x1='", width * 3 / 5, "' x2='", width * 3 / 5, "'></line>");

    html.push(
      "<line class='axis stroke' y1='0' y2='", height,
      "' x1='", width * 4 / 5, "' x2='", width * 4 / 5, "'></line>");


 //   /** render two a y axis after 1/3 and 2/3 of the grid width **/
 //   var y_axis_left = elem.querySelector(".axis.y .stroke.left");
 //   y_axis_left.setAttribute("x1", width * 1 / 3);
 //   y_axis_left.setAttribute("x2", width * 1 / 3);
 //   y_axis_left.setAttribute("y1", 0);
 //   y_axis_left.setAttribute("y2", height);

 //   var y_axis_right = elem.querySelector(".axis.y .stroke.right");
 //   y_axis_right.setAttribute("x1", width * 2 / 3);
 //   y_axis_right.setAttribute("x2", width * 2 / 3);
 //   y_axis_right.setAttribute("y1", 0);
 //   y_axis_right.setAttribute("y2", height);

    html.push("</g>");
    return html.join("");
  }

  /** init **/
  render();
}
