/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Laura Schlimmer, FnordCorp B.V.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

function FnordMetricMetricDetailConfig(params) {

  this.getMetricID = function() {
    return params.route.args.metric_id;
  }

  this.getFilter = function() {
    var filter_param = URLUtil.getParamValue(params.path, "filter");
    return filter_param ? filter_param : null;
  }

  this.getFrom = function() {
    var from_param = URLUtil.getParamValue(params.path, "from");
    return from_param ? from_param : null;
  }

  this.getUntil = function() {
    var until_param = URLUtil.getParamValue(params.path, "until");
    return until_param ? until_param : null;
  }

  this.getTimezone = function() {
    var timezone_cookie = CookieUtil.get("timezone");
    return timezone_cookie ? timezone_cookie : null;
  }

}

