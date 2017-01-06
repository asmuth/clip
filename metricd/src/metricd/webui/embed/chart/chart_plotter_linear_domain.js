/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetricChart.PlotterLinearDomain = function() {

  var min;
  var max;

  this.findMinMax = function(series) {
    var min_values = series.map(function(s) {
      return Math.min.apply(null, s.values)
    });

    this.setMin(Math.min.apply(null, min_values));

    var max_values = series.map(function(s) {
      return Math.max.apply(null, s.values)
    });

    this.setMax(Math.max.apply(null, max_values));
  }

  this.setMin = function(min_value) {
    min = min_value;
  }

  this.setMax = function(max_value) {
    max = max_value;
  }

  this.convertDomainToScreen = function(input_val) {
    if (input_val < min || max - min == 0) {
      return 0;

    } else if (input_val > max) {
      return 1.0

    } else {
      return (input_val - min) / (max - min);

    }

   // returns 0..1
  }

  this.convertScreenToDomain = function(input_val) { // input val is 0..1
    switch (input_val) {
      case 0:
        return min;

      case 1:
        return max;

      default:
        return input_val * (max - min) + min;
    }
   // returns -inf..+inf
  }

}

