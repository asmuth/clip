<?php
/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 * only if attribute future-selectable is set future dates are selectable
 * same holds true for past-selectable
 */

include("../3rdparty/libfnord/fnord-cockpit/fn-cockpit-timenav.html");
include("../3rdparty/libfnord/fnord-webcomponents/components/fn-input.html");
include("../3rdparty/libfnord/fnord-webcomponents/components/fn-datepicker.html");
include("../3rdparty/libfnord/fnord-webcomponents/components/fn-daterangepicker.html");
include("../3rdparty/libfnord/fnord-webcomponents/components/fn-dropdown.html");

class TimeNavigation {

  function __construct() {
    $this->url_query_parts_ = $this->getUrlQueryPartsOrSet();
    $this->end_time_ = $this->url_query_parts_["until_ts"];
    $this->time_range_ =
      $this->url_query_parts_["until_ts"] - $this->url_query_parts_["from_ts"];
  }

  function startTime() {
    return $this->url_query_parts["from_ts"];
  }

  function startTimeMilli() {
    return ($this->url_query_parts_["from_ts"]) * 1000;
  }

  function timeRange() {
    return $this->time_range_;
  }

  function timeRangeMilli() {
    return $this->time_range_ * 1000;
  }

  function endTime() {
    return $this->end_time_;
  }

  function endTimeMilli() {
    return $this->end_time_ * 1000;
  }


  private function getUrlQueryPartsOrSet() {
    $parts = $this->getUrlQueryParts();

    if ($parts["until_ts"] == NULL) {
      $parts["until_ts"] = date(time());
      $update = true;
    }

    if ($parts["from_ts"] == NULL) {
      $parts["from_ts"] = $parts["until_ts"] - 300;
      $update = true;
    }

    return $parts;
  }

  private function getUrlQueryParts() {
    $parts = [];
    parse_str($_SERVER["QUERY_STRING"], $parts);
    return $parts;
  }

}

$timeNav = new TimeNavigation();
?>

<fn-time-navigation
    data-selectable="past" 
    data-from=<?= $timeNav->startTimeMilli(); ?>
    data-until=<?= $timeNav->endTimeMilli(); ?>></fn-time-navigation>
