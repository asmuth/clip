/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

function FnordMetricSeriesListViewConfig(params) {
  var metric_id = params.route.args.metric_id;
  var filter = URLUtil.getParamValue(params.path, "filter") 

  this.getMetricID = function() {
    return metric_id;
    return ;
  }

  this.setFilter = function(filter_value) {
    filter = filter_value;
  }

  this.getFilter = function() {
    return filter;
  }
}

