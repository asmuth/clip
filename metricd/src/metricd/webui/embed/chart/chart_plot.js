/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

/**
 * Renders a chart and optionally a summary for the provided series
 *
 * @param elem   {HTMLElement} an HTML elem within the chart will be rendered
 * @param params {object}      the chart's configuration
 */
FnordMetricChart.Plotter = function(elem, params) {
  'use strict';

  var width = 1240;
  var height = 180;
  var canvas_margin_top = 10;
  var canvas_margin_right = 10;
  var canvas_margin_bottom = 10;
  var canvas_margin_left = 10;
  var x_domain;
  var x_ticks_count = 8;
  var y_domain;
  var y_ticks_count = 5;
  var y_label_width = 50;

  this.render = function(series) {
    /* set up domains */
    var x_domain = new FnordMetricChart.PlotterLinearDomain;
    var x_ticks_count = 8;
    var y_domain = new FnordMetricChart.PlotterLinearDomain; //FIXME allow multiple y_domains

    series.forEach(function(s) {
      y_domain.findMinMax(s.values);
      x_domain.findMinMax(s.time);
    });

    /* set up layout */
    fitLayout();

    /* draw the svg */
    var svg = drawChart();
    console.log(svg);
    elem.innerHTML = svg;
  }

  function fitLayout() {
    /* fit the y axis */
    if (params.axis_y_position == "inside") {

    } else {
      canvas_margin_left = y_label_width;
    }

    /* fit the x axis */
    canvas_margin_bottom += 20;
  }

  function drawChart(c) {
    var svg = new FnordMetricChart.SVGHelper();
    svg.svg += "<svg class='fm-chart' viewBox='0 0 " + width + " " + height + "' >";

    drawBorders(svg);
    drawXAxis(svg);
    drawYAxis(svg);

    svg.svg += "</svg>"
    return svg.svg;
  }

  function drawBorders(c) {
    /** render top border **/
    c.drawLine(
        canvas_margin_left,
        width - canvas_margin_right,
        canvas_margin_top,
        canvas_margin_top,
        "border");

    /** render bottom border  **/
    c.drawLine(
        canvas_margin_left,
        width - canvas_margin_right,
        height - canvas_margin_bottom,
        height - canvas_margin_bottom,
        "border");

    /** render right border  **/
    c.drawLine(
        width - canvas_margin_right,
        width - canvas_margin_right,
        canvas_margin_top,
        height - canvas_margin_bottom,
        "border");

    /** render left border **/
    c.drawLine(
        canvas_margin_left,
        canvas_margin_left,
        canvas_margin_top,
        height - canvas_margin_bottom,
        "border");
  }

  function drawXAxis(c) {
    c.svg += "<g class='axis x'>";

    /** render tick/grid **/
    var text_padding = 3;
    for (var i = 1; i < x_ticks_count; i++) {
      var tick_x_domain = (i / x_ticks_count);
      var tick_x_screen = tick_x_domain * (width - (canvas_margin_left + canvas_margin_right)) + canvas_margin_left;

      c.drawLine(
          tick_x_screen,
          tick_x_screen,
          canvas_margin_top,
          height - canvas_margin_bottom,
          "grid");

      c.drawText(
          tick_x_screen,
          (height - canvas_margin_bottom) + text_padding,
          "blah");
    }

    c.svg += "</g>";
  }

   function drawYAxis(c) {
    c.svg += "<g class='axis y'>";

    /** render labels **/
    //for (var i = 0; i < x_ticks_count + 1; i++) {
    //  c.drawText(
    //      (i / x_ticks_count) * (width - (canvas_margin_left + canvas_margin_right)),
    //      (height - canvas_margin_bottom) + text_padding,
    //      "blah");
    //}

    c.svg += "</g>";
   }

 // function drawLine(series, x_domain, y_domain) {
 //   var line = [];

 //   for (var i = 0; i < series.values.length; i++) {
 //     var x = x_domain.convertDomainToScreen(series.time[i]);
 //     var y = y_domain.convertDomainToScreen(series.values[i]);

 //     line.push(i == 0 ? "M" : "L", x, y);
 //   }

 //   var html = [];
 //   html.push(
 //       "<path class='line' d='", line.join(" "),
 //       "'></path>");

 //   return html.join("");
 // }

 // function formatDate(timestamp) {
 //   function appendLeadingZero(num) {
 //     if (num < 10) {
 //       return "0" + num;
 //     }

 //     return "" + num;
 //   }

 //   var d = new Date(timestamp)
 //   return [
 //       d.getFullYear(), "-",
 //       appendLeadingZero(d.getMonth() + 1), "-",
 //       appendLeadingZero(d.getDate()), " ",
 //       appendLeadingZero(d.getHours()), ":",
 //       appendLeadingZero(d.getMinutes())].join("");
 // }
}

