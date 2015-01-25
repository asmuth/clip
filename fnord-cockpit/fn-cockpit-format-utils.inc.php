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
  return "TS: " . $ts;
}

function fn_format_duration($ts) {
  return "DUR: " . $ts;
}

function fn_format_bytes($bytes) {
  return number_format($bytes / 1000000.0, 2) . "MB";
}

?>
