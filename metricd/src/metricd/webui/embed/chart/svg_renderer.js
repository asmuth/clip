/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetricChart.SVGRenderer = function() {
  var viewbox = false;
  var html = [];

  this.setViewbox = function(vb) {
    viewbox = vb;
  }

  this.beginChart = function(width, height, class_name) {
    //FIXME add viewBox viewBox='0 0 ", width, " ", height, "'
    if (viewbox) {
      append("<svg viewBox='");
    } else {

    }
    return "<svg class='fm-chart'>"

  }

  this.drawLine = function(x1, x2, y1, y2, class_name) {

  }

  this.drawText = function(text, x, y, class_name) {

  }

  this.drawPoint = function(x, y, size, color, class_name) {

  }

  this.drawPath = function(points, class_name) {

  }

  this.beginGroup = function(class_name) {

  }

  this.endGroup = function(class_name) {

  }

  this.getHTML = function() {
    return html.join("");
  }

  function append(str) {
    html.push(str);
  }
}
