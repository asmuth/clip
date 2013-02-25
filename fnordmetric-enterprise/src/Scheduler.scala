// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

class Scheduler extends Thread {

  val TICK = 10

  // this runs in the background and tries to invoke some flushes. it
  // is not neccessary for the liveliness of the application that each
  // iteration of the loop finishes in a fixed time interval. even if
  // this thread dies completely all data will be correctly recorded,
  // only the last sample on each metric won't be flushed until new data
  // arrives
  override def run : Unit =
    while (true) next

  private def next : Unit = {
    val now = FnordMetric.now

    // search for metrics with a pending flush interest and flush them
    // if the now > flush_interest
    for (metric <- MetricFactory.metric_map)
      if (metric._2.flush_interest > 0 && metric._2.flush_interest <= now)
        metric._2.flush_bucket

    // to avoid burning CPU we sleep for a few ms
    Thread.sleep(TICK)
  }

}
