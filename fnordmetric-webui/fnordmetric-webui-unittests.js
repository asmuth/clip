/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.util === undefined) {
  FnordMetric.util = {};
}

/* Unit Tests for FnordMetric.util functions */
FnordMetric.UnitTests = function() {
  console.log("make unit tests");
  var results = {
    total : 0,
    bad : 0
  }

  var testParseQueryString = function() {
    function test(querystr, expected) {
      var result = FnordMetric.util.parseQueryString(querystr);
      result = JSON.stringify(result);
      expected = JSON.stringify(expected);
      if (result != expected) {
        results.bad++;
        console.log("Testing parseQueryString expected "
          + expected + ", but was " + result);
      }
    }
    test("metric_list?metric=/osx/load_avg_15m&view=value", 
        {path : "metric_list",
          query_params : {
            innerView : "metric",
            innerViewValue : "/osx/load_avg_15m",
            view : "value"}});

    test("metric_list", {path : "metric_list", query_params : {}});
    test("query_playground?sql_query=select%20*%20from%20http_status_codes%3B",
      {path : "query_playground",
        query_params : {
          innerView : "sql_query",
          innerViewValue : decodeURIComponent("select%20*%20from%20http_status_codes%3B")}});
    test("", {path : "", query_params: {}});
    test("undefined", {path: "", query_params:{}});
  }();

}
