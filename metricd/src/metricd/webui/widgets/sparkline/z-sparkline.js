/**
  This file is part of the "FnordMetric" project
    Copyright (c) 2015 Laura Schlimmer
    Copyright (c) 2015 Paul Asmuth

  FnordMetric is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License v3.0. You should have received a
  copy of the GNU General Public License along with this program. If not, see
  <http://www.gnu.org/licenses/>.
*/

var SparklineComponent = function() {
  this.createdCallback = function() {
    if (this.hasAttribute('data-sparkline')) {
      this.render();
    }
  };

  this.attributeChangedCallback = function(attr, old_val, new_val) {
    if (attr == 'data-sparkline') {
      this.render();
    }
  };

  this.render = function() {
    var y = {}
    y.values = this.getAttribute('data-sparkline')
      .split(",")
      .map(function(v) {
        return parseFloat(v);
      });

    y.min = (this.hasAttribute('data-min')) ?
      parseFloat(this.getAttribute('data-min')) : 0.0;
    y.max = (this.hasAttribute('data-max')) ?
     parseFloat(this.getattribute('data-max')) : Math.max.apply(null, y.values);

    this.renderWithData({y: y});
  };

  this.renderWithData = function(values, ymin, ymax) {
    var height = this.getDimension('height');
    var width = this.getDimension("width");
    var padding_x = 0;
    var padding_y = 5;

    var points = this.scaleValues(values, ymin, ymax);

    var svg_line = [];
    for (var i = 0; i < points.length; ++i) {
      if (!isNaN(points[i].y)) {
        var dx = padding_x + (points[i].x * (width - padding_x * 2));
        var dy = padding_y + ((1.0 - points[i].y) * (height - padding_y * 2));
        svg_line.push(i == 0 ? "M" : "L", dx, dy);
      }
    }

    var tpl = templateUtil.getTemplate("z-sparkline-tpl");

    this.innerHTML = "";
    this.appendChild(tpl);

    var svg = this.querySelector("svg");
    svg.style.height = height + "px";
    svg.style.width = width + "px";

    var path = this.querySelector("path");
    path.setAttribute("d", svg_line.join(" "));
  };

  this.getDimension = function(dimension) {
    var value = this.getAttribute(dimension);

    var idx = value.indexOf("%");
    if (idx > -1) {
      return this.offsetWidth * (parseInt(value.substr(0, idx), 10) / 100);
    }

    return parseFloat(value);
  };

  this.scaleValues = function(axes) {
    var scaled = [];

    for (var i = 0; i < axes.y.values.length; ++i) {
      var v = axes.y.values[i];
      var x  = i / (axes.y.values.length - 1);

      if (v < axes.y.min) {
        scaled.push({x : x, y: 0});
        continue;
      }

      if (v > axes.y.max) {
        scaled.push({x: x, y: 1.0});
        continue;
      }

      if (axes.y.max - axes.y.min == 0) {
        scaled.push({x: x, y: v});
      } else {
        scaled.push({x: x, y:(v - axes.y.min) / (axes.y.max - axes.y.min)});
      }
    }

    return scaled;
  };
};

var proto = Object.create(HTMLElement.prototype);
SparklineComponent.apply(proto);
document.registerElement("z-sparkline", { prototype: proto });
