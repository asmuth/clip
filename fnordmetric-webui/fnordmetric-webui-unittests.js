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
      results.total++;
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

  var testSetUrlQueryString = function() {
    function test(hash, query_params, encode, push_state, expected) {
      results.total++;
      FnordMetric.util.setURLQueryString(hash, query_params, encode, push_state);
      var result = window.location.hash;
      if (result != expected) {
        results.bad++;
        console.log("Testing setURLQueryString expected "
          + expected + ", but was " + result);
      }
    }
    test("metric_list", {}, false, false, "#metric_list");
    test(undefined,
        {innerView : "metric",
          innerViewValue : "http_status_codes"}, 
        false,
        false,
        "");

    test("metric_list", 
        {innerView : undefined,
          innerViewValue : "http_status_codes"},
        false,
        false,
        "#metric_list");

    test("query_playground",
          {innerView : "sql_query",
          innerViewValue : "SELECT * FROM `http_status_codes`;"},
          true,
          true,
          "#query_playground?sql_query=SELECT%20*%20FROM%20%60http_status_codes%60%3B");
    test("", {}, false, false, "");
  }();

  var testParseTimestamp = function() {
    function test(timestamp, expected) {
      results.total++;
      var result = FnordMetric.util.parseTimestamp(timestamp);
      if (result.substr(0,13) != expected) {
        results.bad++;
        console.log("Testing parseTimestamp expected " +
          expected + ", but was " + result.substr(0,12));
      }
    }
    var now = Date.now();
    test(now, "0 seconds ago");
    test(now *1000, "0 seconds ago")
    test(Math.floor(now / 1000), "0 seconds ago");
  }();

  var testSortMetricList = function() {
    function test(metrics, column_index, order, expected) {
      results.total++;
      FnordMetric.util.sortMetricList(metrics, column_index, order);
      var result = metrics;
      if (result[0][column_index] != expected[0][column_index]) {
        results.bad++;
        console.log("Testing sortMetricList expected "
          + expected[0][column_index] + ", but was " + result[0][column_index]);
      }
    }
    var metric1 = [
      "/osx/load_avg_15m", "host", "5 hours ago - Nov 18 2014 11:33:11", 10693668];
    var metric2 = [
      "/osx/load_avg_5m", "", "5 hours ago - Nov 18 2014 11:33:12", 10693669];

    test([metric1, metric2], 0, "asc", [metric1, metric2]);
    test([metric1, metric2], 0, "desc", [metric2, metric1]);
    test([metric1, metric1], 0, "asc", [metric1, metric1]);
    test([metric1, metric2], 2, "desc", [metric2, metric1]);
    test([metric1, metric2], 2, "asc", [metric1, metric2]);
    test([metric1, metric2], 1, "desc", [metric1, metric2]);

  }();

  var testSearchMetricList = function() {
    function test(metrics, search_item, expected) {
      results.total++;
      var result = FnordMetric.util.searchMetricList(metrics, search_item);
      for (var i = 0; i < result.length; i++) {
        if (JSON.stringify(result[i]) != JSON.stringify(expected[i])) {
          results.bad++;
          console.log("Testing setURLQueryString expected "
            + expected[i] + ", but was " + result[i]);
        }
      }
    }
    var m1 = {key :  "/osx/load_avg_15m", labels : "", last_insert : 123};
    var m2 = {key : "/osx/load 15m", labels : "" };
    var m3 = {key : undefined, labels : ""};
    var metrics = [m1, m2, m3];

    test(metrics, "15", [m1, m2]);
    test(metrics, 15, [m1, m2]);

  }();

  console.log(results.total + " tests, "+  results.bad + " failed.");

}
