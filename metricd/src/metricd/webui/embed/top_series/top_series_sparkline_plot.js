/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetricTopSeries.SparklinePlotter = function(elem, params) {
  'use strict';

  var width = elem.offsetWidth * 1.6 || 180; //FIXME
  var height = 14 * 1.6;
  var canvas_margin_top = 1;
  var canvas_margin_right = 1;
  var canvas_margin_bottom = 1;
  var canvas_margin_left = 1;

  var y_domain = new FnordMetricTopSeries.SparklineDomain();

  this.render = function(series) {
    y_domain.findMinMax(series.values);

    draw(series);

    /* adjust the svg when the window is resized */
    window.addEventListener("resize", function(e) {
      width = elem.offsetWidth;
      draw(series);
    }, false);
  }

/********************************** private ***********************************/

  function draw(series) {
    var svg = drawSparkline(series);
    elem.innerHTML = svg;
  }

  function drawSparkline(series) {
    var svg = new FnordMetric.SVGHelper();
    svg.svg += "<svg shape-rendering='geometricPrecision' class='fm-chart' viewBox='0 0 " + width + " " + height + "' >";

    drawLine(series, svg);

    svg.svg += "</svg>";
    return svg.svg;
  }

  function drawLine(series, svg) {
    var points = [];

    for (var i = 0; i < series.values.length; i++) {
      var x = i / (series.values.length - 1);
      var x_screen = x * (width - (canvas_margin_left + canvas_margin_right)) + canvas_margin_left;

      if (series.values[i] === null) {
        points.push([x_screen, null]);
      } else {
        var y = y_domain.convertDomainToScreen(series.values[i]);
        var y_screen = (1.0 - y) * (height - (canvas_margin_bottom + canvas_margin_top)) + canvas_margin_top;
        points.push([x_screen, y_screen]);
      }
    }

    svg.drawPath(points, "line");
  }

}

