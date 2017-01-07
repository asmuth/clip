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

  this.getMetricID = function() {
    return params.route.args.metric_id;
  }

  this.getFilter = function() {
    var filter_param = URLUtil.getParamValue(params.path, "filter");
    return filter_param ? filter_param : null;
  }

  this.getStart = function() {
    var start_param = URLUtil.getParamValue(params.path, "start");
    return start_param ? start_param : null;
  }

  this.getEnd = function() {
    var end_param = URLUtil.getParamValue(params.path, "end");
    return end_param ? end_param : null;
  }

  this.getTimezone = function() {
    var timezone_cookie = DOMUtil.getCookie("timezone");
    return timezone_cookie ? timezone_cookie : null;
  }

}

