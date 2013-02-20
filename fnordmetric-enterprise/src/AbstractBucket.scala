// FnordMetric Enterprise
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

package com.fnordmetric.enterprise

trait AbstractBucket {

  var next_flush : Long = 0

  def sample(value: Double) : Unit
  def flush() : Double

  def flush_every(interval: Long, proc: (Long, Double) => Unit) = {
    val now = FnordMetric.now

    if (next_flush == 0)
      next_flush = now

    while (next_flush <= now) {
      proc(next_flush, flush)
      next_flush += interval
    }
  }

}
