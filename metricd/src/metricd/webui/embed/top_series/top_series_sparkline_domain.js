/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

FnordMetricTopSeries.SparklineDomain = function() {

  var min = null;
  var max = null;

  this.findMinMax = function(values) {
    this.setMin(Math.min.apply(null, values));
    this.setMax(Math.max.apply(null, values));
  }

  this.setMin = function(min_value) {
    min = min_value;
  }

  this.setMax = function(max_value) {
    max = max_value;
  }

  this.convertDomainToScreen = function(input_val) {
    if (input_val < min) {
      return 0;
    }

    if (input_val > max) {
      return 1.0
    }

    return (input_val - min) / (max - min);
  }
}

