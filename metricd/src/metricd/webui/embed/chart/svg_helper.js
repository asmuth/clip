/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetricChart.SVGHelper = function() {
  'use strict';

  this.svg = "";

  this.drawLine = function(x0, x1, y0, y1, classes) {
    this.svg +=
        "<line" +
            xmlAttr("x1", x0) +
            xmlAttr("x2", x1) +
            xmlAttr("y1", y0) +
            xmlAttr("y2", y1) +
            xmlAttr("class", classes) +
        " />";
  }

  this.drawText = function(x, y, text, classes) {
    this.svg +=
        "<text" +
            xmlAttr("x", x) +
            xmlAttr("y", y) +
            xmlAttr("class", classes) +
        ">" +
            xmlEscape(text) +
        "</text>";
  }

  function xmlAttr(name, value) {
    return " " + name + "='" +  value + "'"; // FIXME WARNING: does not escape...
  }

  function xmlEscape(val) {
    // FIXME WARNING: does not escape...
    return val;
  }

}

