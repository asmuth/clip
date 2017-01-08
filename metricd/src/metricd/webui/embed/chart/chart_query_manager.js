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
FnordMetricChart.QueryManager = function(params) {

  var callback_success;
  var callback_error;
  var success;
  var queries_total;
  var queries_completed;
  var result = {
    series: []
  }

  this.executeQueries = function(on_succes, on_error) {
    var fetch_opts = {
      metric_id: params.metric_id
    };

    callback_success = on_succes;
    callback_error = on_error;
    success = true;
    series = [];
    queries_total = params.queries.length;
    queries_completed = 0;

    params.queries.forEach(function(qry) {
      if (qry.query.op == "fetch_summary") {
        executeFetchSummary(qry.query);
        return;
      }
    });
  }

  function executeFetchSummary(qry) {
    delete qry.op;
    qry.from = params.from;
    qry.until = params.until;

    HTTPUtil.httpPost(
        "/api/v1/metrics/fetch_summary",
        JSON.stringify(qry),
        {},
        function(r) {
      if (r.status != 200) {
        success = false;
        return;
      }

      try {
        var r = JSON.parse(r.response);
        result.unit = r.unit;
        r.series.forEach(function(s) {
          result.series.push(s);
        });
      } catch (e) {
        success = false;
      }

      triggerQueryDone();
    });
  }

  function triggerQueryDone() {
    if (!success && callback_error) {
      callback_error();
      callback_error = null;
    }

    queries_completed += 1;
    if (queries_completed == queries_total) {
      callback_success(result);
    }
  }

};

