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
FnordMetricUnits = {};

// FIXME
FnordMetricUnits.chooseUnit = function(unit, values) {
  if (!unit) {
    return null;
  }

  var unit_scale = unit.unit_scale || 1;
  var values_max = Math.max.apply(null, values) * unit_scale;

  var candidates = [];
  for (k in unit.names) {
    candidates.push([k, unit.names[k].factor]);
  }

  candidates.sort(function(a, b) {
    return b[1] - a[1];
  });

  var idx = 0;
  for (; idx < candidates.length; ++idx) {
    if (candidates[idx][1] < values_max) {
      break;
    }
  }

  return candidates[idx][0];
}

FnordMetricUnits.formatValue = function(unit, value) {
  return FnordMetricUnits.formatValues(unit, [value]);
}

FnordMetricUnits.formatValues = function(unit, values) {
  var unit_name = FnordMetricUnits.chooseUnit(unit, values);
  return FnordMetricUnits.formatValuesWithUnit(unit, unit_name, values)
}

FnordMetricUnits.formatValuesWithUnit = function(unit, unit_name, values) {
  return values.map(function(v) {
    return FnordMetricUnits.formatValueWithUnit(unit, unit_name, v);
  });
}

FnordMetricUnits.formatValueWithUnit = function(unit, unit_name, value) {
  if (!unit || !unit_name) {
    return value.toFixed(2);
  }

  var unit_scale = unit.unit_scale || 1;
  var unit_name = unit.names[unit_name];
  return ((value * unit_scale) / unit_name.factor).toFixed(1) + unit_name.symbol;
}

