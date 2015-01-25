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

function fn_format_timestamp($ts) {
  $date = new DateTime();
  $date->setTimestamp(get_unix_timestamp(intval($ts)));
  return $date->format('Y-m-d H:i:s');
}

function get_unix_timestamp($ts) {
  //2100
  while ($ts > 4099680000) {
    $ts = $ts / 1000;
  }
  return $ts;
}

function fn_format_duration($ts, $seconds) {
  $dur = $ts / $seconds;
  if ($dur < 60) {
    return $dur . "s";
  } else {
    return $dur / 60 . "m";
  }
}

function fn_format_bytes($bytes) {
  return number_format($bytes / 1000000.0, 2) . "MB";
}

?>
