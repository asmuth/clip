/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V.
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

  var width;
  var height;
  var canvas_margin_top;
  var canvas_margin_right;
  var canvas_margin_bottom;
  var canvas_margin_left;
  var x_domain;
  var x_ticks_count;
  var y_ticks_count;
  var y_domain;
  var y_label_width;
  var y_labels;

  this.render = function(result) {
    width = elem.offsetWidth * 1.6;
    height = 180 * 1.6;

    /* prepare axes */
    prepareXAxis(result);
    prepareYAxis(result);

    /* prepare layout */
    prepareLayout(result);

    /* draw the svg */
    draw(result);

    /* adjust the svg when the window is resized */
    window.addEventListener("resize", function(e) {
      width = elem.offsetWidth;
      draw(result);
    }, false);
  }

  function prepareLayout(result) {
    canvas_margin_top = 10;
    canvas_margin_right = 1;
    canvas_margin_bottom = 30;
    canvas_margin_left = 1;

    /* fit the y axis labels */
    y_label_width =
        12 *
        Math.max.apply(null, y_labels.map(function(l) { return l.length; }));

    /* fit the y axis */
    if (params.axis_y_position == "inside") {

    } else {
      canvas_margin_left = y_label_width;
    }
  }

  function prepareXAxis(result) {
    x_ticks_count = 12;
    x_domain = new FnordMetricChart.PlotterLinearDomain;

    result.series.forEach(function(s) {
      x_domain.findMinMax(s.time);
    });
  }

  function prepareYAxis(result) {
    y_ticks_count = 5;
    y_domain = new FnordMetricChart.PlotterLinearDomain;

    result.series.forEach(function(s) {
      y_domain.findMinMax(s.values);
    });

    /* set up y axis labels */
    var y_values = []
    for (var i = 0; i <= y_ticks_count ; i++) {
      y_values.push(y_domain.convertScreenToDomain(1.0 - (i / y_ticks_count)));
    }

    y_labels = FnordMetricUnits.formatValues(result.unit, y_values);
  }

  function draw(result) {
    var svg = drawChart(result);
    elem.innerHTML = svg;
  }

  function drawChart(result) {
    var svg = new FnordMetric.SVGHelper();
    svg.svg += "<svg shape-rendering='geometricPrecision' class='fm-chart' viewBox='0 0 " + width + " " + height + "' >";

    drawBorders(svg);
    drawXAxis(svg);
    drawYAxis(result, svg);

    result.series.forEach(function(s) {
      drawLine(s, svg);

      if (params.points) {
        drawPoints(s, svg);
      }
    });

    svg.svg += "</svg>"
    return svg.svg;
  }

  function drawBorders(c) {
    /** render top border **/
    if (params.border_top) {
      c.drawLine(
          canvas_margin_left,
          width - canvas_margin_right,
          canvas_margin_top,
          canvas_margin_top,
          "border");
    }

    /** render right border  **/
    if (params.border_right) {
      c.drawLine(
          width - canvas_margin_right,
          width - canvas_margin_right,
          canvas_margin_top,
          height - canvas_margin_bottom,
          "border");
    }

    /** render bottom border  **/
    if (params.border_bottom) {
      c.drawLine(
          canvas_margin_left,
          width - canvas_margin_right,
          height - canvas_margin_bottom,
          height - canvas_margin_bottom,
          "border");
    }

    /** render left border **/
    if (params.border_left) {
      c.drawLine(
          canvas_margin_left,
          canvas_margin_left,
          canvas_margin_top,
          height - canvas_margin_bottom,
          "border");
    }
  }

  function drawXAxis(c) {
    c.svg += "<g class='axis x'>";

    /** render tick/grid **/
    var text_padding = 10;
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
          "2017-01-01");
    }

    c.svg += "</g>";
  }

   function drawYAxis(result, c) {
    c.svg += "<g class='axis y'>";

    /** render tick/grid **/
    for (var i = 0; i <= y_ticks_count ; i++) {
      var tick_y_domain = (i / y_ticks_count);
      var tick_y_screen = tick_y_domain * (height - (canvas_margin_bottom + canvas_margin_top)) + canvas_margin_top;

      c.drawLine(
          canvas_margin_left,
          width - canvas_margin_right,
          tick_y_screen,
          tick_y_screen,
          "grid");

      if (params.axis_y_position == "inside" && (i == y_ticks_count)) {
        /* skip text */
      } else if (params.axis_y_position == "inside") {
        var text_padding = 3;
        c.drawText(
            canvas_margin_left + text_padding,
            tick_y_screen,
            y_labels[i],
            "inside");
      } else {
        var text_padding = 8;
        c.drawText(
            canvas_margin_left - text_padding,
            tick_y_screen,
            y_labels[i],
            "outside");
      }
    }

    c.svg += "</g>";
   }

  function drawLine(series, c) {
    var points = [];

    for (var i = 0; i < series.values.length; i++) {
      var x = x_domain.convertDomainToScreen(series.time[i]);
      var x_screen = x * (width - (canvas_margin_left + canvas_margin_right)) + canvas_margin_left;

      if (series.values[i] === null) {
        points.push([x_screen, null]);
      } else {
        var y = y_domain.convertDomainToScreen(series.values[i]);
        var y_screen = (1.0 - y) * (height - (canvas_margin_bottom + canvas_margin_top)) + canvas_margin_top;
        points.push([x_screen, y_screen]);
      }
    }

    c.drawPath(points, "line");
  }

  function drawPoints(series, c) {
    var point_size = 3;

    for (var i = 0; i < series.values.length; i++) {
      var x = x_domain.convertDomainToScreen(series.time[i]);
      var y = y_domain.convertDomainToScreen(series.values[i]);

      var x_screen = x * (width - (canvas_margin_left + canvas_margin_right)) + canvas_margin_left;
      var y_screen = height - (y * (height - (canvas_margin_bottom + canvas_margin_top)) + canvas_margin_bottom);

      c.drawPoint(x_screen, y_screen, point_size, "point");
    }
  }

}

