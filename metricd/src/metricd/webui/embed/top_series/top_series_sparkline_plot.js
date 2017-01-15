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

  var RENDER_SCALE_FACTOR = 1.6;
  var width;
  var height;
  var canvas_margin_top = 1;
  var canvas_margin_right = 1;
  var canvas_margin_bottom = 1;
  var canvas_margin_left = 1;
  var y_domain;

  this.render = function(series) {
    width = (params.width || elem.offsetWidth) * RENDER_SCALE_FACTOR;
    height = (params.height || elem.offsetHeight) * RENDER_SCALE_FACTOR;

    y_domain = new FnordMetricTopSeries.SparklineDomain();
    y_domain.findMinMax(series.values);

    draw(series);
  }

/********************************** private ***********************************/

  function draw(series) {
    var svg = drawSparkline(series);
    elem.innerHTML = svg;
  }

  function drawSparkline(series) {
    var svg = new FnordMetric.SVGHelper();
    svg.svg += "<svg shape-rendering='geometricPrecision' class='fm-chart' viewBox='0 0 " + width + " " + height + "' style='width:" + width + "px;'>";

    drawLine(series, svg);

    svg.svg += "</svg>";
    return svg.svg;
  }

  function drawLine(series, svg) {
    var points = [];

    for (var i = 0; i < series.values.length; i++) {
      var x = i / (series.values.length - 1);
      var x_screen = x * (width - (canvas_margin_left + canvas_margin_right)) + canvas_margin_left;

      var y = 0;
      if (series.values[i] !== null) {
        y = y_domain.convertDomainToScreen(series.values[i]);
      }

      var y_screen = (1.0 - y) * (height - (canvas_margin_bottom + canvas_margin_top)) + canvas_margin_top;
      points.push([x_screen, y_screen]);
    }

    svg.drawPath(points, "line", { stroke: FnordMetric.Colors.default[0] });
  }

}

