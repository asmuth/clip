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

function curl_ezmulti($urls) {
  $channels = array();

  foreach ($urls as $url) {
    $channel = curl_init();
    curl_setopt($channel, CURLOPT_URL, $url);
    curl_setopt($channel, CURLOPT_HEADER, 0);
    curl_setopt($channel, CURLOPT_RETURNTRANSFER, 1);
    $channels[] = $channel;
  }

  $mh = curl_multi_init();

  foreach ($channels as $channel) {
    curl_multi_add_handle($mh, $channel);
  }

  $active = null;
  do {
    $mrc = curl_multi_exec($mh, $active);
  } while ($mrc == CURLM_CALL_MULTI_PERFORM);

  while ($active && $mrc == CURLM_OK) {
    curl_multi_select($mh);

    do {
      $mrc = curl_multi_exec($mh, $active);
    } while ($mrc == CURLM_CALL_MULTI_PERFORM);
  }

  $bodies = array();
  foreach ($channels as $channel) {
    $bodies[] = curl_multi_getcontent($channel);
  }

  foreach ($channels as $channel) {
    curl_multi_remove_handle($mh, $channel);
  }

  curl_multi_close($mh);
  return $bodies;
}

?>
